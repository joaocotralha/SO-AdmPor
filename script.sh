#!/bin/sh

#compare two files (ctrl.s & test.s) and find the number of differences
equals () {
  count=$(diff ctrl.s test.s | grep ">" | wc -l)

  if [ "$count" -eq 0 ]
  then
    printf "EQUAL\n"
  else
    printf "DIFFERENT\nDIFFERENCES:$count\n"
  fi

  rm -f ctrl.s
  rm -f test.s
}

ctrl=$(find . -name $1)
shift
test=$(find . -name $1)

if [ -r "$ctrl" ] && [ -r "$test" ]
then
  sort $ctrl > ctrl.s
  sort $test > test.s
  equals
else
  printf "File does not exist"
fi
