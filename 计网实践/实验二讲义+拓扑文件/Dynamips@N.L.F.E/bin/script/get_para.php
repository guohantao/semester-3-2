<?php

/*******************************************************/
/* Dynamips ��������                                   */
/* ���������ڻ���������������Ը�ʽ����ʾ              */
/* ver 2                                               */
/* Program by Stanely.WY                               */
/*******************************************************/

echo "\r\n";
@chdir('..\\workingdir\\');
$line=@exec("..\\bin\\dynamips\\dynamips-wxp.exe -e", $str);
$p='/^\s*(rpcap:)(.*)/';
$i=0;

foreach ($str as $key => $var) {
    if (preg_match($p, $var)) {
        $i++;
        if ($i!=1) {
            $var=trim($var);

            $svar=explode(':', $var);
            //print_r($svar);

            echo "��������\t\t\t\t\t    �����ͺ�\r\n";
            $var1=substr($svar[1],strpos($svar[1],'\\'),strlen($svar[1]));
            $var2=substr($svar[2],strpos($svar[2],'\'')+1,28);
            echo "$var1 $var2\r\n";
        }
    };
}
?>
