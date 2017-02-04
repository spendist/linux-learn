#!/usr/bin/php -q

<?PHP

	$redis = new Redis();
	$redis->connect('127.0.0.1',6379);
	if ( !$redis) { die("Can't connect localhost 6379 port!\n"); }

	$fp=popen("/opt/vc/bin/vcgencmd measure_temp","r");
	$str=split('=',fread($fp,128));
	pclose($fp);
	if (count($str)==2) {
		$key=$str[0];
		$value=$str[1];
	}
	// print_r($str);


	$redis = new Redis();
	$redis->connect('127.0.0.1',6379);
	if ( !$redis) { die("Can't connect localhost 6379 port!\n"); }

	if ( $redis->exists($key)==1 ) {
		echo "has this key\n";
		$ans=$redis->get($key);
	} else {
		$redis->set($key,$value);
	}

	$redis->expire($key,30);  // Delete this key after 30s, when run this program , return null.

	if ( isset($ans) ) echo "ans:$key=$ans\n";





?>
	
