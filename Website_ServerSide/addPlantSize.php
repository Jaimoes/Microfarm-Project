<?php
   	include("connect.php");
   	
   	$link=Connection();
	
	if($_SERVER['REMOTE_ADDR'] == "87.214.245.168") { // Use with online Server
	
		$plantSize = (isset($_GET["plantSize"]) ? $_GET["plantSize"] : null);
	
		$plantSize = mysqli_real_escape_string($link, $_GET['plantSize']);

		$query = "INSERT INTO `microfarm0plantSize` (`time`, `plantSize`) 
			VALUES (NOW(), '".$plantSize."')"; 
   	
		//echo $query;
		mysqli_query($link, $query);
		mysqli_close($link);
	}
 
?>