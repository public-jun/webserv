#!/usr/bin/perl
# 上の１行の前には空行も空白文字もはいらないようにしてください。
# perlのパス名はプロバイダや環境に合わせて変更してください。

#
# CGIのヘッダを書き出します。通常は text/html を指定します。
#
print "Content-type: text/html; charset=utf-8\n";
print "\n";

#
# HTMLを書き出します。
# "～\n" の ～ の部分に HTML を記述しますが、ダブルクォーテーション( " )
# を用いる際は " の代わりに ' を用いるか、\" のように、バックスラッシュ( \ )
# を前に置いてください。
#
print "<HTML>\n";
print "<HEAD>\n";
print "<TITLE>CGI TEST</TITLE>\n";
print "</HEAD>\n";
print "<BODY BGCOLOR='#FFFFFF' TEXT='#000000'>\n";
print "<XMP>\n";

#
# wwwperl.cgi?引数1+引数2 で指定したコマンド引数を書き出します。
#
print "=================================\n";
print "コマンド引数\n";
print "=================================\n";
for ($i = 0; $i <= $#ARGV; $i++) {
	print "ARGV[$i] = [ $ARGV[$i] ]\n";
}
print "\n";

#
# CGIスクリプトが参照可能な環境変数を書き出します。
#
print "=================================\n";
print "環境変数\n";
print "=================================\n";
print "AUTH_TYPE = [ $ENV{'AUTH_TYPE'} ]\n";
print "CONTENT_LENGTH = [ $ENV{'CONTENT_LENGTH'} ]\n";
print "CONTENT_TYPE = [ $ENV{'CONTENT_TYPE'} ]\n";
print "GATEWAY_INTERFACE = [ $ENV{'GATEWAY_INTERFACE'} ]\n";
print "PATH_INFO = [ $ENV{'PATH_INFO'} ]\n";
print "PATH_TRANSLATED = [ $ENV{'PATH_TRANSLATED'} ]\n";
print "QUERY_STRING = [ $ENV{'QUERY_STRING'} ]\n";
print "REMOTE_ADDR = [ $ENV{'REMOTE_ADDR'} ]\n";
print "REMOTE_IDENT = [ $ENV{'REMOTE_IDENT'} ]\n";
print "REMOTE_USER = [ $ENV{'REMOTE_USER'} ]\n";
print "REQUEST_METHOD = [ $ENV{'REQUEST_METHOD'} ]\n";
print "REQUEST_URI = [ $ENV{'REQUEST_URI'} ]\n";
print "SCRIPT_NAME = [ $ENV{'SCRIPT_NAME'} ]\n";
print "SERVER_NAME = [ $ENV{'SERVER_NAME'} ]\n";
print "SERVER_PORT = [ $ENV{'SERVER_PORT'} ]\n";
print "SERVER_PROTOCOL = [ $ENV{'SERVER_PROTOCOL'} ]\n";
print "SERVER_SOFTWARE = [ $ENV{'SERVER_SOFTWARE'} ]\n";
print "\n";

#
# フォームに指定した値を読み込んで、書き出します。
#
print "=================================\n";
print "フォーム変数\n";
print "=================================\n";
if ($ENV{'REQUEST_METHOD'} eq "POST") {
	# POSTであれば標準入力から読込みます
	read(STDIN, $query_string, $ENV{'CONTENT_LENGTH'});
	print "$query_string\n";
}
else {
	# GETであれば環境変数から読込みます
	$query_string = $ENV{'QUERY_STRING'};
#}
# 「変数名1=値1&変数名2=値2」の形式をアンパサンド( & )で分解します
@a = split(/&/, $query_string);
# それぞれの「変数名=値」について
foreach $a (@a) {
	# イコール( = )で分解します
	($name, $value) = split(/=/, $a);
	# + や %8A などをデコードします
	$value =~ tr/+/ /;
	$value =~ s/%([0-9a-fA-F][0-9a-fA-F])/pack("C", hex($1))/eg;
	# 変数名と値を書き出します
	print "$name = $value\n";
	# 後で使用する場合は、$FORM{'変数名'} に代入しておきます
	$FORM{$name} = $value;
}
}

# HTMLの終わりの部分を書き出します。
#
print "</XMP>\n";
print "</BODY>\n";
print "</HTML>\n";
print "\n";
