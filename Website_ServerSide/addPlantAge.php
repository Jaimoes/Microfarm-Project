<?php
   	include("connect.php");
   	
   	$link=Connection();
	
	if($_SERVER['REMOTE_ADDR'] == "87.214.245.168") { // Use with online Server
	
		$plantAge = (isset($_GET["plantAge"]) ? $_GET["plantAge"] : null);
	
		$plantAge = mysqli_real_escape_string($link, $_GET['plantAge']);

		$query = "INSERT INTO `microfarm0irrigationt` (`time`, `plantAge`) 
			VALUES (NOW(), '".$plantAge."')"; 
   	
		//echo $query;
		mysqli_query($link, $query);
		mysqli_close($link);
	}
 
?>