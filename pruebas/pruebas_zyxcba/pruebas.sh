#!/usr/bin/env bash

set -eu

PROGRAMA="$1"
VALGRIND="valgrind --leak-check=full --track-origins=yes --error-exitcode=2"

RET=0
OUT=`mktemp`
trap "rm -f $OUT" EXIT

for x in *_in; do
  b=${x%_in}
  echo -n "Prueba $b... "

  ($PROGRAMA ${b}_doctores ${b}_pacientes <${b}_in || RET=$?) |
    diff -u --label "${b}_cátedra" --label "${b}_estudiante" ${b}_out - >$OUT || :

  if [[ $RET -ne 0 ]]; then
    echo -e "programa abortó con código $RET.\n\nValgrind:"
    $VALGRIND $PROGRAMA ${b}_doctores ${b}_pacientes <${b}_in
    exit $RET

  elif [[ -s $OUT ]]; then
    echo -e "output incorrecto:\n"
    cat $OUT
    exit 1

  else
    if [ $x == "06_in" ]; then 
      echo -e "OK."
    else
      echo -e "OK.\n\nValgrind:"
      $VALGRIND $PROGRAMA ${b}_doctores ${b}_pacientes <${b}_in >/dev/null
    fi
  fi
  echo
done
