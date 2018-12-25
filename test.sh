#!/bin/bash
rake compile
#irb -Ilib -rgeneprog
ruby -Ilib -rgeneprog -e "
  puts 'creating gene'
  gene = Geneprog::CGPGene.new(3,3,3)
  puts 'gene:'
  puts gene
  puts 'evaluating...'
  result = gene.evaluate([1,2,3])
  puts 'result:'
  puts result"
