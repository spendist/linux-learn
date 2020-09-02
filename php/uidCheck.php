<?PHP

	$AreaCodes=array();

function getAreacode($filename="areacode.csv")
{
	global $AreaCodes;

	$fp=fopen($filename,"r");
	$buf=fgetcsv($fp,512,"\t");
	while ( !feof($fp) ) {
		if ( strlen($buf[0])<1) break;
		$AreaCodes[$buf[0]]=$buf[1];
		$buf=fgetcsv($fp,512,"\t");
	}
	fclose($fp);
	// var_dump($AreaCodes);
}


function CheckUID($id)
{

	global $AreaCodes;


	// 返回一个结构体
	$info=array(
		'error'=>0,
		'errmsg'=>'',
		'areacode'=>'',
		'address'=>'',
		'birthday'=>'',
		'age'=>0,
		'sex'=>''
		);

	$error=0; // 8 bit error
	$sexes='01234567890';
	$ceces='0123456789X';

	$id=strtoupper($id);
	$len=strlen($id);

	// 位数不对,(18 or 15)
	if ($len!=15 && $len!=18) { 
		$info['error']=1;
		$info['errmsg']='身份证位数不正确|'.$id;
		return $info;
	}
	$strLeft=substr($id,0,-1); $strCec=substr($id,-1);
	// 字符串匹配
	$len=strlen($id);
	$times=($len>15 ? 16 : 13 );
	$pattern='/^[0-9]\d{'.$times.'}$/';
	$x=preg_match($pattern, $strLeft,$matches);
	// 前部分不正确 ,夹杂有非法字符  
	//if ($x==0) { $error=2; return $error; }
	// CEC部分不正确 ,夹杂有非法字符
	$y=preg_match('/[0-9Xx]/',$strCec,$matches);
	// if ($y==0) { $error=3; return $error; }
	if ($x==0 || $y==0 ) {
		$info['error']=2;
		$info['errmsg']='身份证中包含非法字符|'.$id;
		return $info;
	}


	if ( strlen($id)==15) {
		// 长度自动升级
		// 看后三位： 996,997,999 为百岁以上老人
		$r3=(int)substr($id,-3);
		if ( $r3==996 || $r3==997 || $r3==999 ) { $BigOne=TRUE; } else {$BigOne=FALSE;}

		$year=(int)substr($id,6,2);
		if ($BigOne==TRUE ) { $year+=1800; }
		if ($year<32) {$year+=2000; } else { $year+=1900; }
		$left=substr($id,0,6);
		$right=substr($id,-7);
		$id=sprintf('%s%d%s',$left,$year,$right);
		$cecChar=Cal_Idchecksum($id);
		$id=$id.$cecChar;
	}
	$areacode=substr($id,0,6); 
	$info['areacode']=$areacode;
	if (!array_key_exists($areacode,$AreaCodes)) { 
		$info['error']=3;
		$info['errmsg']='找不到身份证中包含的行政地区'.$areacode;
		return $info;
	}
	$info['address']=$AreaCodes[$areacode];
	$sex=substr($id,16,1) ; // 奇数，男性, 偶数为女性 
	if ( ((int)$sex)%2==0 ) $info['sex']='Girl'; else $info['sex']='Boy';
	$cec=substr($id,17,1);  // cec 
	// 生日不是一个日期
	if (mktime(0,0,0,
		(int)substr($id,10,2),
		(int)substr($id,12,2),
		(int)substr($id,6,4))==FALSE ) { 
		$info['error']=4;
		$info['errmsg']="身份证日期字段非法|".$substr($id,6,8);
		return $info; 
	}
	$info['birthday']=substr($id,6,8);
	$age=(int)date('Y')-(int)substr($id,6,4);
	$info['age']=$age;
	// cal cec 
	$cec2=Cal_Idchecksum($id);
	if ( strcmp($cec,(string)$cec2)!=0 ) { 
		$info['error']=5;
		$info['errmsg']="身份证校验码非法|".$cec.'V'.$cec2;
		return $info; 
	}
	return $info;
}

function Cal_Idchecksum($id)
{
	$factor=array(7,9,10,5,8,4,2,1,6,3,7,9,10,5,8,4,2);
	$CEC=array('1','0','X','9','8','7','6','5','4','3','2');
	$checksum=0;
	for ( $i=0; $i<17; $i++) {
		$v=(int)substr($id,$i,1);
		$checksum+=$v*$factor[$i];
	}
	$mod=$checksum % 11;
	return $CEC[$mod];
}

/****************************************************************************
* main function
******************************************************************************/

	getAreacode("areacode.csv");


	$ret=CheckUID('Abda32720514481');    print_r($ret);echo "\n";
	$ret=CheckUID('61213272051448p');    print_r($ret);echo "\n";
	$ret=CheckUID('61213272051448');     print_r($ret);echo "\n";
	$ret=CheckUID('612132720514481');    print_r($ret);echo "\n";
	$ret=CheckUID('612132720514482');    print_r($ret);echo "\n";
	$ret=CheckUID('61213219a205144811'); print_r($ret);echo "\n";
	$ret=CheckUID('612132237205144811'); print_r($ret);echo "\n";
	$ret=CheckUID('90213219720f144811'); print_r($ret);echo "\n";
	$ret=CheckUID('6121321972051448g1'); print_r($ret);echo "\n";
	$ret=CheckUID('61213219720514481X'); print_r($ret);echo "\n";
	$ret=CheckUID('612132197205144811'); print_r($ret);echo "\n";
	$ret=CheckUID('62262719850120181X'); print_r($ret);echo "\n";
	$ret=CheckUID('510122197901030270'); print_r($ret);echo "\n";

?>
