(* $Id: bigint.ml,v 1.5 2014-11-11 15:06:24-08 - - $ *)

open Printf

module Bigint = struct

    type sign     = Pos | Neg
    type bigint   = Bigint of sign * int list
    let  radix    = 10
    let  radixlen =  1

    let car       = List.hd
    let cdr       = List.tl
    let map       = List.map
    let reverse   = List.rev
    let lslen     = List.length
    let strcat    = String.concat
    let strlen    = String.length
    let strsub    = String.sub
    let zero      = Bigint (Pos, [])

    let charlist_of_string str = 
        let last = strlen str - 1
        in  let rec charlist pos result =
            if pos < 0
            then result
            else charlist (pos - 1) (str.[pos] :: result)
        in  charlist last []

    let bigint_of_string str =
        let len = strlen str
        in  let to_intlist first =
                let substr = strsub str first (len - first) in
                let digit char = int_of_char char - int_of_char '0' in
                map digit (reverse (charlist_of_string substr))
            in  if   len = 0
                then zero
                else if   str.[0] = '_'
                     then Bigint (Neg, to_intlist 1)
                     else Bigint (Pos, to_intlist 0)

    let string_of_bigint (Bigint (sign, value)) =
        match value with
        | []    -> "0"
        | value -> let reversed = reverse value
                   in  strcat ""
                       ((if sign = Pos then "" else "-") ::
                        (map string_of_int reversed))

    let rec cmp list1 list2 = 
        if (lslen list1) > (lslen list2) then 1
        else if (lslen list1) < (lslen list2) then -1
        else match (list1, list2) with
            | [], []          -> 0
            | list1, []       -> 1
            | [], list2       -> -1
            | list1, list2 ->  
                let li1 = reverse list1 in
                let li2 = reverse list2 in
                    if (car li1) > (car li2) then 1
                    else if (car li1) < (car li2) then -1
                    else cmp (reverse (cdr li1)) (reverse (cdr li2))

    let rec trimzero' list = 
	if list = [] then [0]
	else if car (reverse list) = 0 then trimzero' (reverse(cdr(reverse list)))
	else list

    let rec add' list1 list2 carry = match (list1, list2, carry) with
        | list1, [], 0       -> list1
        | [], list2, 0       -> list2
        | list1, [], carry   -> add' list1 [carry] 0
        | [], list2, carry   -> add' [carry] list2 0
        | car1::cdr1, car2::cdr2, carry ->
          let sum = car1 + car2 + carry
          in  sum mod radix :: add' cdr1 cdr2 (sum / radix)

    let rec sub' list1 list2 carry = match (list1, list2, carry) with
        | list1, [], carry   -> if carry = 0 then list1
		else let subt = (car list1) - 1 in
			if subt >= 0 then subt::(cdr list1)
			else (subt + radix)::(sub' (cdr list1) [] 1)
        | [], list2, carry   -> failwith "arg1 must > arg2"
        | car1::cdr1, car2::cdr2, carry -> 
            let sum = car1 - car2 in
                if carry = 1 then sub' ((car1 - 1)::cdr1) list2 0
                else if sum < 0 then (sum + radix)::(sub' cdr1 cdr2 1)
                else sum::(sub' cdr1 cdr2 0)
    
    let double value = add' value value 0


    let rec mul' multiplier powerof2 multiplicand' =
        if multiplier = [0] then failwith "zero operation"
	else if (cmp powerof2 multiplier) > 0 then []
  	else let result = powerof2::multiplicand'::[] in
  		result::mul' (trimzero'(multiplier))
		    (double powerof2) (double multiplicand')


    let rec mult' num list =
	if cmp num (car(car list)) = 0
	    then car(cdr(car list))
	else if cmp (car(car list)) num > 0
	    then mult' num (cdr list)
	else let resu = car(cdr(car list)) in
		add' resu (mult' (trimzero' (sub' num (car(car list)) 0)) (cdr list)) 0

    let rec div' dividend divisor leftcol=
	if divisor = [0] then failwith "divide by 0"
	else if cmp divisor dividend > 0 then []
	else let result = leftcol::divisor::[] in
		result::div' dividend (double divisor) (double leftcol)

    let rec divi' num list =
	if list = [] then [0]
	else if cmp (car(cdr(car list))) num = 0 then car(car list)
	else if cmp (car(cdr(car list))) num > 0 then divi' num (cdr list)
	else let resu = car(car list) in
	    add' resu (divi' (trimzero'(sub' num (car(cdr(car list))) 0)) (cdr list)) 0

    let rec rem' num list =
	if list = [] then num
	else if cmp (car(cdr(car list))) num = 0 then [0]
	else if cmp (car(cdr(car list))) num > 0 then rem' num (cdr list)
	else rem' (trimzero'(sub' num (car(cdr(car list))) 0)) (cdr list)

    let rec pow' list1 list2 =
	if cmp list2 [0] < 0 then [0]
	else if list2 = [0] then [1]
	else let result = list1 in
		mult' result (reverse(mul' 
			result [1] (pow' list1 (trimzero'(sub' list2 [1] 0)))))

    let flip sign1 =
	if sign1 = Pos then Neg
	else Pos

    let add (Bigint (neg1, value1)) (Bigint (neg2, value2)) =
        if neg1 = neg2
        then Bigint (neg1, add' value1 value2 0)
        else let res = cmp value1 value2 in
	    if res > 0 then Bigint (neg1, sub' value1 value2 0)
	    else if res < 0 then Bigint (neg2, trimzero'(sub' value2 value1 0))
	    else zero

    let sub (Bigint (neg1, value1)) (Bigint (neg2, value2)) =
	if neg1 <> neg2
        then Bigint (neg1, add' value1 value2 0)
        else let res = cmp value1 value2 in
            if res > 0 then Bigint (neg1, trimzero' (sub' value1 value2 0))
            else if res < 0 then Bigint ((flip neg1), trimzero' (sub' value2 value1 0))
            else zero
		

    let mul (Bigint (neg1, value1)) (Bigint (neg2, value2)) =
	if neg1 = neg2
	then Bigint (Pos, mult' value1 (reverse(mul' value1 [1] value2)))
	else Bigint (Neg, mult' value1 (reverse(mul' value1 [1] value2)))

    let div (Bigint (neg1, value1)) (Bigint (neg2, value2)) = 
	if neg1 = neg2
	then Bigint (Pos, divi' value1 (reverse(div' value1 (trimzero' value2) [1])))
	else Bigint (Neg, divi' value1 (reverse(div' value1 (trimzero' value2) [1])))

    let rem (Bigint (neg1, value1)) (Bigint (neg2, value2)) = 
	Bigint (Pos, rem' value1 (reverse(div' value1 value2 [1])))

    let pow (Bigint (neg1, value1)) (Bigint (neg2, value2)) = 
	if neg2 = Neg then zero
	else if (rem' value2 (reverse(div' value2 [2] [1]))) = [0] then 
	    Bigint (Pos, pow' value1 value2)
	else Bigint (neg1, pow' value1 value2)

end

