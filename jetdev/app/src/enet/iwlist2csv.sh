#!/bin/sh

dev=$1 # e.g)ra0
OUTPUT=$2 # fileName

GREP_KWD='Cell\|SSID\|Quality\|Encryption'
EXTRACT_CELL_NO='s/.*Cell\s\([0-9]*\).*$/\1/g' # No
EXTRACT_ESSID='s/.*\"\(.*\)\".*$/,\[\1\]/g' # ESSID
EXTRACT_QUALITY='s/.*Quality=\([0-9]*\)\/\([0-9]*\).*$/,\1,\2/g' # Quality,QualityBase
EXTRACT_ENCRYPTION='s/.*Encryption\skey:\(.*\)/,\1/g' # Encryption
RMV_SPACE='/^\s*$/d' # 空白行削除

ret=$(ifconfig | grep $dev)

if [ ${#ret} -gt 0 ] ; then # ifconfigのgrep結果が0文字より多い＝$devがup状態
  iwlist $dev scanning \
  | grep $GREP_KWD \
  | sed -e $EXTRACT_CELL_NO -e $EXTRACT_ESSID -e $EXTRACT_QUALITY -e $EXTRACT_ENCRYPTION -e $RMV_SPACE \
  | sed -e ':loop; N; $!b loop; ;s/\n,/,/g' 1> $OUTPUT
else
  rm -rf $OUTPUT
  touch  $OUTPUT
fi
