<?php
   	include("connect.php");
   	
   	$link=Connection();
	
	//if($_SERVER['REMOTE_ADDR'] == "87.209.22.67") { // Use with online Server
	
		$isOn = (isset($_GET["isOn"]) ? $_GET["isOn"] : null);
	
		$isOn = mysqli_real_escape_string($link, $_GET['isOn']);

		$query = "INSERT INTO `microfarm0irrigationt` (`time`, `pumpOn`) 
			VALUES (NOW(), '".$isOn."')"; 
   	
		//echo $query;
		mysqli_query($link, $query);
		mysqli_close($link);
	//}
 
?>
