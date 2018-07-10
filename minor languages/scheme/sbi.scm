#!/afs/cats.ucsc.edu/courses/cmps112-wm/usr/racket/bin/mzscheme -qr
;; $Id: sbi.scm,v 1.3 2016-09-23 18:23:20-07 - - $
;;
;; NAME
;;    sbi.scm - silly basic interpreter
;;
;; SYNOPSIS
;;    sbi.scm filename.sbir
;;
;; DESCRIPTION
;;    The file mentioned in argv[1] is read and assumed to be an SBIR
;;    program, which is the executed.  Currently it is only printed.
;;
(define *stderr* (current-error-port))

(define *run-file*
    (let-values
        (((dirpath basepath root?)
            (split-path (find-system-path 'run-file))))
        (path->string basepath))
)

(define (die list)
    (for-each (lambda (item) (display item *stderr*)) list)
    (newline *stderr*)
    (exit 1)
)

(define (usage-exit)
    (die `("Usage: " ,*run-file* " filename"))
)

(define (readlist-from-inputfile filename)
    (let ((inputfile (open-input-file filename)))
         (if (not (input-port? inputfile))
             (die `(,*run-file* ": " ,filename ": open failed"))
             (let ((program (read inputfile)))
                  (close-input-port inputfile)
                         program))))

;;==============================================================
;;==============================================================

(define *function-table* (make-hash))
(define (function-get key)
        (hash-ref *function-table* key))
(define (function-put! key value)
        (hash-set! *function-table* key value))

(define *label-table* (make-hash))
(define (label-get key)
        (hash-ref *label-table* key))
(define (label-put! key value)
        (hash-set! *label-table* key value))

(define *variable-table* (make-hash))
(define (variable-get key)
        (hash-ref *variable-table* key))
(define (variable-put! key value)
        (hash-set! *variable-table* key value))

(define (evaluate expr)    
	(cond 
	   ((pair? expr) 
		(apply (function-get (car expr)) (map evaluate (cdr expr))))
	   ((number? expr) expr)               
           (else (variable-get expr))
	)
)

(define (dim-statement array)
	(variable-put! (caar array) (make-vector (evaluate(cadar array))))
	(function-put! (caar array) 
		(lambda (x) (vector-ref (variable-get (caar array)) (- x 1)))
	)
)

(define (let-statement expr)
	(if (not (pair? (car expr)))
		(variable-put! (car expr) (evaluate (cadr expr)))
		(vector-set! (variable-get (caar expr))
			     (- (evaluate (cadar expr)) 1) 
			     (evaluate (cadr expr))
		)
	)
)

(define (print-statement string)
	(if (null? string)
		(newline)
		(begin  
		   (if (string? (car string))
		   	(display (car string))
			(display (evaluate (car string)))
		   )
		   (print-statement (cdr string))
                )
	)
)

(define (input-statement expr)
	(let ((input (read)))
	     (variable-put! (car expr) input)
	     (variable-put! `inputcount (+ (variable-get `inputcount) 1 ))
	     (when (not (null? (cdr expr)))
             	  (input-statement (cdr expr))
	     )
        )
)


(for-each
    (lambda (pair)
            (variable-put! (car pair) (cadr pair)))
    `(
        (pi      3.141592653589793238462643383279502884197169399)
        (e       2.718281828459045235360287471352662497757247093)
	(inputcount 0)
     )
)

(for-each
    (lambda (pair)
            (function-put! (car pair) (cadr pair)))
    `(

        (log10_2 0.301029995663981195213738894724493026768189881)
        (sqrt_2  1.414213562373095048801688724209698078569671875)
        (e       2.718281828459045235360287471352662497757247093)
        (pi      3.141592653589793238462643383279502884197169399)
        (div     ,(lambda (x y) (floor (/ x y))))
        (log10   ,(lambda (x) (/ (log x) (log 10.0))))
        ;;(mod     ,(lambda (x y) (- x (* (div x y) y))))
        (quot    ,(lambda (x y) (truncate (/ x y))))
        ;;(rem     ,(lambda (x y) (- x (* (quot x y) y))))
        (+       ,+)
	(-       ,-)
	(*       ,*)
	(/       ,(lambda (x y) (/ x (if (= y 0) 0.0 y))))
        (^       ,expt)
	(=       ,=)
	(<       ,<)
	(>       ,>)
	(<=      ,<=)
	(>=      ,>=)
	(<>      ,(lambda(x y) (not (= x y))))
        (ceil    ,ceiling)
        (exp     ,exp)
        (floor   ,floor)
        (log     ,(lambda (x) (log (if (= x 0) 0.0 x))))
        (sqrt    ,sqrt)
	(tan	 ,tan)
	(cos	 ,cos)
	(sin	 ,sin)
	(atan	 ,atan)
	(acos	 ,acos)
	(asin	 ,asin)
	(abs	 ,abs)
	(round   ,round)
	(dim     ,dim-statement)
	(let     ,let-statement)
	(goto    ,null)
	(if	 ,null)
	(print   ,print-statement)
	(input   ,input-statement)

     ))


(define (put-in-label list)
        (when (not (null? list))
              (let ((first (caar list)))
                   (when (number? first)
			(if (null? (cdar list))
				(void)
				(if (symbol? (cadar list))
					(label-put! (cadar list) (caar list))
					(void)
				)
			)
                   )
              )
              (put-in-label (cdr list))
	)
)

(define (run-function label list current-line)
	(cond ((eqv? (car label) 'goto)
      		(interpret list (- (label-get (cadr label)) 1))) 
    	      ((eqv? (car label) 'if)
      		(if (evaluate(cadr label))
        		(interpret list (- (label-get (caddr label)) 1))
        		(interpret list (+ current-line 1)))
              )
              (else
      		((function-get (car label)) (cdr label))
      		(interpret list (+ current-line 1))
    	      )
	)
)

(define (interpret list current-line)
	(when (< current-line (length list))
	     (let ((line (list-ref list current-line)))
                  (cond ((= (length line) 3) 
                         (set! line (cddr line))
                         (run-function (car line) list current-line)
                        )
                        ((and (= (length line) 2) (list? (cadr line)))
                         (set! line (cdr line))
                         (run-function (car line) list current-line)
                        )           
                        (else
                         (interpret list (+ current-line 1)))
                  )
	     )
	)
)

(define (main arglist)
    (if (or (null? arglist) (not (null? (cdr arglist))))
        (usage-exit)
        (let* ((sbprogfile (car arglist))
               (program (readlist-from-inputfile sbprogfile)))
		(begin (put-in-label program)
                       (interpret program 0)
		;;(display program
         ))))

(main (vector->list (current-command-line-arguments)))
