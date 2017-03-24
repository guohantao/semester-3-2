<?php

/*******************************************************/
/* Dynamips 辅助工具                                   */
/* 本程序能够自动的更改labini目录下所有的拓扑配置文件  */
/* 的网卡参数                                          */
/*                                                     */
/* ver 2.0                                             */
/* Program by Stanely.WY                               */
/*******************************************************/

$dir='../net/';
$net_card_info=$argv[1];

function es_parse_str($s,$n) {
    $p='/^\s*([0-9]+)\s*=\s*access\s*([0-9]+)\s*NIO_gen_eth:(.*)/';
    if (preg_match($p, $s)) {
        $s=str_replace(substr($s,strrpos($s,':')+1,strlen($s)), $n, $s);
        return $s."\r\n";
    }
    return $s;
}

$dh = opendir($dir);
while (($file = readdir($dh)) !== false) {
	if ($file=='.' || $file=='..') {
		continue;
	}

	$temp[]=$dir.$file;
	$c=file("$dir$file");
    foreach ($c as $k => $v) {
        $v=es_parse_str($v,$net_card_info);
        $c[$k]=$v;
    }
    $c=implode("",$c);

    $fp=fopen($dir.$file,'w');
    fwrite($fp,$c);
    fclose($fp);
}

echo "\r\n\r\n";
echo "已经成功修改了如下拓扑文件:\r\n";
foreach ($temp as $k => $v) {
    echo $v."\r\n";
}
echo "\r\n\r\n";

?>