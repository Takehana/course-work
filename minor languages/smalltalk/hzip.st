#!/usr/local/bin/gst -f
"$Id: catfile.st,v 1.11 2012-03-02 15:10:13-08 - - $"
execname := '$RCSfile: catfile.st,v $' =~ '.*: (.*),v.*' at: 1.

"============================================================="
"         file"
"============================================================="
FileStream extend [
   nl := Character nl.
   catFile [
      |count|
      count := 0.
      [self atEnd not] whileTrue: [
         count := count + 1.
         count asString size to: 5 do: [:skip| stdout << ' '].
         stdout << count << ': ' << self nextLine << nl.
      ]
   ]
]

Character extend [
   isPrint [
      ^ (Character space <= self) & (self <= $~)
   ]
   visible [
      self isPrint ifTrue: [^ self asString]
                   ifFalse: [^ self asInteger printStringRadix: 8]
   ]
]

"==============================================================="
"        charfreq"
"==============================================================="
Object subclass: ZeroArray [
   |theArray|

   ZeroArray class >> new [
      self shouldNotImplement.
   ]

   ZeroArray class >> new: size [
      |result|
      result := super new.
      result init: size.
      ^ result
   ]

   init: size [
      theArray := Array new: size.
   ]

   size [
      ^ theArray size.
   ]

   at: index [
      ^ theArray at: index + 1.
   ]

   at: index put: value [
      ^ theArray at: index + 1 put: value.
   ]

   incr: index [
      (self at: index) isNil
         ifTrue: [ self at: index put: 0].
      self at: index put: (self at: index) + 1.
   ]

   keysAndValuesDo: aBlock [
      (0 to: (self size) - 1) do: [:index |
         aBlock value: index value: (self at: index).
      ]
   ]

]
"==================================================================="
"              tree"
"==================================================================="
Object subclass: Leaf [
   |char count isLeaf|
   char [ ^ char ]
   count [ ^ count ]
   isLeaf [^ isLeaf ]

   Leaf class >> new [
      self shouldNotImplement
   ]

   Leaf class >> new: aChar count: aCount [
      |result|
      result := super new.
      result setChar: aChar andCount: aCount.
      ^result
   ]

   setChar: aChar andCount: aCount [
      char := aChar.
      count := aCount.
      isLeaf := true.
   ]

   <= other [
      ^ (count < other count)
      | ((count = other count) & (char <= other char))
   ]

   printBase: aStream [
      ^ aStream << self class << '(' << char << ',' << count
   ]

   printOn: aStream [
      (self printBase: aStream) << ')'.
   ]

   inorder: visitor prefix: string [
      visitor value: char value: string.
   ]

   postorder: visitor [
      visitor value: char value: isLeaf.
   ]

]


Leaf subclass: Tree [
   |left right|

   Tree class >> new: aChar count: aCount [
      self shouldNotImplement
   ]

   Tree class >> new: aChar count: aCount left: aLeft right: aRight [
      |result|
      result := super new: aChar count: aCount.
      result setLeft: aLeft andRight: aRight.
      ^ result
   ]

   setLeft: aLeft andRight: aRight [
      left := aLeft.
      right := aRight.
      isLeaf := false.
   ]

   printOn: aStream [
      (self printBase: aStream) << ',' << left << ',' << right << ')'.
   ]

   inorder: visitor prefix: string [
      left inorder: visitor prefix: string, '0'.
      right inorder: visitor prefix: string, '1'.
   ]

   postorder: visitor [      
      left postorder: visitor.
      right postorder: visitor.  
      visitor value: char value: isLeaf.
   ]

   getleft [
      ^ left
   ]
   
   getright [
      ^ right
   ]
]

"==================================================================="
"       write bits"
"==================================================================="
Object subclass: OutBits [
   |bitIndex currentByte myStream|
   OutBits class >> new [
      self shouldNotImplement.
   ]
   OutBits class >> new: fileStream [
      |result|
      result := super new.
      result init: fileStream.
      ^result
   ]
   clearByte [
      bitIndex := 8.
      currentByte := 0.
   ]
   init: fileStream [
      myStream := fileStream.
      self clearByte.
   ]
   flushByte [
      bitIndex = 8 ifFalse: [
         myStream nextPutByte: currentByte.
         self clearByte.
      ]
   ]
   writeBit: bit [
      currentByte := currentByte bitAt: bitIndex put: bit.
      bitIndex := bitIndex - 1.
      bitIndex = 0 ifTrue: [self flushByte].
   ]
]

Object subclass: BitStream [
   |bitIndex byte myStream|
   BitStream class >> new: fileStream [
      |result|
      result := super new.
      result init: fileStream.
      ^result
   ]
   init: fileStream [
      myStream := fileStream.
      bitIndex := 1.
   ]
   nextBit [
      bitIndex = 1 ifTrue: [
         byte := myStream next.
         bitIndex := 9.
      ].
      bitIndex := bitIndex - 1.
      ^byte value bitAt: bitIndex
   ]
   atEnd [
      ^bitIndex = 1 and: [myStream atEnd]
   ]
   currByte [
      ^byte
   ]
]

"==================================================================="
"       main"
"==================================================================="
nl := Character nl.

freqtable := ZeroArray new: 257.
freqtable at: 256 put: 1.
freqlist := SortedCollection new.
codetable := ZeroArray new: 257.

cmdt := false.
cmdc := false.
cmdu := false.
argc := 0.

|infile outfile codetree|

Smalltalk arguments do: [:filename |
   (argc = 0) ifTrue: [
	(filename = '-t') ifTrue: [cmdt := true.].
	(filename = '-c') ifTrue: [cmdc := true.].
	(filename = '-u') ifTrue: [cmdu := true.]. ].
   (argc = 1) ifTrue: [
        infile := FileStream open: filename
			     mode: FileStream read. 
        infile2 := FileStream open: filename
                              mode: FileStream read. ].
   (argc = 2) ifTrue: [
        outfile := FileStream open: filename
			      mode: FileStream write. ].
   argc := argc + 1.
].

"
stdout << '==========================' << nl.
stdout << 'input ——> ' << infile << nl.
stdout << 'input2 ——> ' << infile2 << nl.
stdout << 'output ——> ' << outfile << nl.
stdout << '==========================' << nl.
"

[infile atEnd not] whileTrue: [
    |ordChar|
    ordChar := infile next asInteger.
    freqtable incr: ordChar.
].

freqtable keysAndValuesDo: [:key :value |
    (value notNil and: [value > 0]) ifTrue: [
        |entry|
        entry := Leaf new: key count: value.
        freqlist add: entry.
        "stdout << 'freq[' << key asCharacter visible << '—> ' 
            << key << ']=' << value << nl."
    ]
].

[freqlist size > 1] whileTrue: [
    |left right|
    left := freqlist removeFirst.
    right := freqlist removeFirst.
    tree := (Tree new: (left char) count: (left count) + (right count)
             left: left right: right).
    freqlist add: tree.
].
codetree := freqlist removeFirst.

codetree inorder: [:index :string |
    codetable at: index put: string.
] prefix: ''.


bintree := ''.
codetree postorder: [:char :isLeaf |
    isLeaf ifFalse: [bintree := bintree, '1'.]
           ifTrue: [
               bin := ''.
               (char = 256) ifTrue: [bin := '000000001']
                            ifFalse: [
               (char = 0) ifTrue: [bin := '000000000']
                          ifFalse: [
               1 to: 8 by: 1 do: [:i |
                   bin := ((char bitAt:i)printString), bin. ]
               ].
               ].
               bintree := bintree, '0', bin.
           ].
].    
"stdout << bintree << nl."

cmdt ifTrue: [
   freqtable keysAndValuesDo: [:key :value |
      (value notNil and: [value > 0]) ifTrue: [
          |keyname valstr|
          (key = 256) 
          ifTrue: [keyname := 'EOF'.]
          ifFalse: [
          (key < 16)  
          ifTrue: [keyname := 'x0', (key printString: 16).]
          ifFalse: [
          (key < 33)
          ifTrue: [keyname := 'x', (key printString: 16).]
          ifFalse: [
          (key > 126)
          ifTrue: [keyname := 'x', (key printString: 16).]
          ifFalse: [keyname := (key asCharacter visible).
             ].
             ].
             ].
             ].
          (keyname size < 3) ifTrue: [keyname := ' ', keyname.].
          [keyname size < 3] whileTrue: [keyname := keyname, ' '.].
          valstr := value printString.
          [valstr size < 5] whileTrue: [valstr := ' ', valstr.].
          stdout << keyname << '   ' << valstr 
             << '  ' << (codetable at: key) << nl.].
   ].
].

cmdc ifTrue: [
   |outBytes|
   outBytes := OutBits new: outfile.

   1 to: (bintree size) by: 1 do: [:index |
      bit := ((bintree at: index) asInteger) - 48.
      outBytes writeBit: bit.
   ].

   outBytes writeBit: 1.

   [infile2 atEnd not] whileTrue: [
      |ordChar|
      ordChar := infile2 next asInteger.
      encodedSeq := codetable at: ordChar.
      "stdout << ordChar << ' ——> ' << encodedSeq << nl."
      1 to: (encodedSeq size) by: 1 do: [:index |
         bit := ((encodedSeq at: index) asInteger) - 48.
         outBytes writeBit: bit. 
      ].
    ].

    eof := codetable at: 256.
    1 to: (eof size) by: 1 do: [:index |
      bit := ((eof at: index) asInteger) - 48.
      outBytes writeBit: bit.
    ].
    
    outBytes flushByte.
].

cmdu ifTrue: [
   |outBytes stack startDecom| 
   outBytes := OutBits new: outfile.
   stack := OrderedCollection new.
   input := BitStream new: infile2.
   startDecom := false.
   decodetree := ''.
   visit := ''.
   nodeID := 0.
  
   [input atEnd not] whileTrue: [
      |inputbit|
      inputbit := input nextBit.
      "stdout << 'read —> ' << inputbit << nl."

   (startDecom = true) ifFalse: [
      (inputbit = 0) ifTrue: [
         leaf := 0.
         8 to: 1 by: -1 do: [:index |
            bit := input nextBit.
            leaf := leaf bitAt: index put: bit. 
         ].
         (leaf = 0) ifTrue: [
            nextbit := input nextBit.
            (nextbit = 1) ifTrue: [
               leaf := leaf bitAt: 9 put: 1. ].
         ].
         entry := Leaf new: leaf count: ''.
         stack add: entry.
      ] ifFalse: [
         (stack size > 1) ifTrue: [
            stack add: 1.
            ex := stack removeLast.
            right := stack removeLast.
            left := stack removeLast.
            subtree := (Tree new: nodeID count: -1
               left: left right: right).
            nodeID := nodeID + 1.
            stack add: subtree.
         ] ifFalse: [
            decodetree := stack removeLast.
            visit := decodetree.
            startDecom := true.
            "stdout << 'decode now start' << nl."
            inputbit := input nextBit.
         ].
      ].
   ]. 
   (startDecom = true) ifTrue: [
      "stdout << 'now visit ——> ' << (visit char) << ' '
             << 'input —-> ' << inputbit << nl."
      (visit isLeaf) ifTrue: [
            (outfile ~= nil) ifTrue: [
               8 to: 1 by: -1 do: [:index |
                  bit := (visit char) bitAt: index.
                  "stdout << bit."
                  outBytes writeBit: bit.
               ].
            ] ifFalse: [
               outChar := visit char.
               (outChar = 256) ifFalse: [
                  stdout nextPut: (Character value: outChar).
               ]
            ].
            "stdout << nl."
            visit := decodetree. 
      ] ifFalse: [
         (inputbit = 0) ifTrue: [
            visit := visit getleft.
         ] ifFalse: [
            visit := visit getright.
         ].
         (visit isLeaf) ifTrue: [
            "stdout << 'now visit ——> ' << (visit char) << '———> '."
            (outfile ~= nil) ifTrue: [
               8 to: 1 by: -1 do: [:index |
                  bit := (visit char) bitAt: index.
                  "stdout << bit."
                  outBytes writeBit: bit.
               ].
         ] ifFalse: [
            outChar := visit char.
            (outChar = 256) ifFalse: [
               stdout nextPut: (Character value: outChar).
            ]
         ].
         "stdout << nl."
         visit := decodetree.
         ].
      ]. "isleaf t/f"
   ]. "decom t/f"
   ]. "input end t/f"
   "decodetree inspect."
]. 

(infile ~= nil) ifTrue: [infile close.].
(infile2 ~= nil) ifTrue: [infile2 close.].
(outfile ~= nil) ifTrue: [outfile close.].