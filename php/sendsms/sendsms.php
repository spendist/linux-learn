<html>
<head>
<title>����</title>
<body>
<!-- ����˳��򣬲��� b.c ,�ڱ�Ŀ¼���У���һ��linux����������-->
<h1> sms send report </h1><br>

<?PHP
	for ($i=0;$i<256;$i++) echo " ";  // ��ֹIE�����յ������ַ�ʱ��ʲôҲ����ʾ
	//error_reporting("E_ALL ^ E_NOTICE ^ E_WARNING");
	error_reporting("E_ALL");
// ����ҳfrome�л�ȡUser��ͷ�����б���������û��嵥������������
	$UserList=array();  // ��ʼ��������
	//var_dump($_POST);
	while (list($key,$val)=each($_POST)) {
		//echo "key=$key ==> val=$val \n";
		if ( preg_match("/^1\d{10}$/",$val) && strcmp(substr($key,0,4),"User")==0 ) array_push($UserList,$val);
		//if ( preg_match("/^1\d{10}$/",$val) ) echo $val."<br>\n";
	}
	

	echo "<pre>���͸������û���\n";
	var_dump($UserList);
	echo "</pre>!!!!!<br>\n";
	
	// ����������
	if ( count($UserList)<1 ) {
		$UserList=array("1330852072"); // ���û��ѡ���κ��û������͵�ȱʡ�û� ����
	}
	
		
	if ( strlen($_POST["Content"])<1 ) {
		$smsContent="���Ͷ��ŵ��ˣ�����û�и�����д�κζ������Զ���ӱ�����Ϣ��"; 	//��һ�����Ǵ��ļ��ж��룬��Ҫ���������ơ�
	} else {
		$smsContent=$_POST["Content"];
	}

// ��ȡ������������Ϣ
	$confFile="sendsms.ini";   // ������ʹ�õ������ļ�����

	if ( !file_exists($confFile)) die( $confFile ." does not exist, about!");
	if ( !is_file($confFile)) die( $confFile ." is not a normal file, about!");
	if ( !is_readable($confFile)) die( $confFile ." is not readable, about!");

	$confFile=parse_ini_file($confFile,true);  // ���������ļ��е�����

	$smsServer=$confFile["Server"]["serverIP"];
	$smsPort=$confFile["Server"]["port"];
	$smsDepartment=$confFile["Server"]["departmentID"];
	$smsType=$confFile["Server"]["messageType"];
	$smsOperator=$confFile["Server"]["operatorID"];
	// ���ò������
	if (!isset($smsServer))     die("Can't get [Server IP] from configure file ".$confFile);
	if (!isset($smsPort)  )     die("Can't get [Server Port] from configure file ".$confFile);
	if (!isset($smsDepartment)) die("Can't get [send department id] from configure file ".$confFile);
	if (!isset($smsType))       die("Can't get [sms message type] from configure file ".$confFile);
	if (!isset($smsOperator))   die("Can't get [sms send operator id ] from configure file ".$confFile);
	if (!isset($smsContent))    die("No message to send��");


/*	
// ��ʼ��ÿ���˷�����Ϣ
	$numUser=count($UserList);
	for ($i=0; $i<$numUser; $i++) {
		$s=socket_create(AF_INET,SOCK_STREAM,SOL_TCP);
		if (!$s) die("socket_create");
		if (!socket_set_option($s, SOL_SOCKET, SO_SNDTIMEO, array("sec"=>1,"usec"=>0))) die("socket_set_option:".socket_strerror(socket_last_error($s)));
		if (!socket_set_option($s, SOL_SOCKET, SO_RCVTIMEO, array("sec"=>1,"usec"=>0))) die("socket_set_option:".socket_strerror(socket_last_error($s)));
		if (!socket_set_option($s, SOL_SOCKET, SO_REUSEADDR, 1)) die("socket_set_option:".socket_strerror(socket_last_error($s)));
		if (!socket_connect($s,$smsServer,intval($smsPort))) die("socket_connect():".socket_strerror(socket_last_error($s)));
        $smsSendto=array_pop($UserList);
        echo "send message to: [$smsSendto]<br>\nmsg=[".$smsContent."]<br>\n";
        $now=date("YmdHis");  // ��ָ����ʽ���
        $text ="<eix>";
        $text.="<TrCode>700001</TrCode>";
        $text.="<MsgType>3</MsgType>";
        $text.="<SendTime>$now</SendTime>";
        $text.="<SendFlag>1</SendFlag>";
        $text.="<Mobile>".$smsSendto."</Mobile>";
        $text.="<RetScript>$smsContent</RetScript>";
        $text.="<Operator>111111</Operator>";
        $text.="<Branch>$smsDepartment</Branch>";
        $text.="</eix>";

        if ( strcmp( strtoupper($confFile["Other"]["log"]) ,"ON")==0) {
           $fp=fopen($confFile["Other"]["logfile"],"a");
           if (!$fp) die("Write logfile error: ".$confFile["Other"]["logfile"]);
           fwrite($fp,date("Y-m-d H:i:s").$text."\n");
           fclose($fp);
        }
        // echo "\tDebug packet=".$text."\n";
        if (! socket_write($s,$text) ) die("socket_write():".socket_strerror(socket_last_error($s)));
        else echo "...Write OK! <br>\n";
        //ob_clean();ob_flush();
        $recvs=socket_read($s,1024,PHP_BINARY_READ);
        echo "Server return:[<B>$recvs</B>]<br>\n";
      	socket_close($s);
	}  // end of each user;
*/
	echo "<p><hr size=3></p><br>\n";	
	echo "<p><center><a href=\"".$_SERVER['HTTP_REFERER']."\"><strong>�������ǰһҳ</strong></a></p>\n</body></html>\n";
	return 0;	



function parse_Sendto_List($sendto="")
{ // �������ļ��ж�ȡ���͵��Ŀͻ��嵥������������

	$userList=array();

	if (strlen($sendto)<10) return null;
	$users=explode(',',$sendto);
	if (count($users)<1) return null;
	for ($i=0; $i<count($users); $i++) {
		$userdetail=explode(':',$users[$i]);
		$userList[$i]=$userdetail;
	}
	if ( count($userList)<1) return null;
	
	return $userList;
}


?>

</body>