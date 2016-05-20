<?php
   	include("connect.php");
   	
   	$link=Connection();

	if($_SERVER['REMOTE_ADDR'] == "87.214.245.168") { // Use with online Server
		
		
		$tO=(isset($_GET["tO"]) ? $_GET["tO"] : null);
		$hO=(isset($_GET["hO"]) ? $_GET["hO"] : null);
		$tI=(isset($_GET["tI"]) ? $_GET["tI"] : null);
		$hI=(isset($_GET["hI"]) ? $_GET["hI"] : null);
		$tS=(isset($_GET["tS"]) ? $_GET["tS"] : null);
		$hS=(isset($_GET["hS"]) ? $_GET["hS"] : null);
		
		// escape variables for security
		$tO = mysqli_real_escape_string($link, $_GET['tO']);
		$hO = mysqli_real_escape_string($link, $_GET['hO']);
		$tI = mysqli_real_escape_string($link, $_GET['tI']);
		$hI = mysqli_real_escape_string($link, $_GET['hI']);
		$tS = mysqli_real_escape_string($link, $_GET['tS']);
		$hS = mysqli_real_escape_string($link, $_GET['hS']);
	

		$query = "INSERT INTO `microfarm0climate` (`time`, `tempOutside`, `humOutside`, `tempInside`, `humInside`, `tempSoil`, `humSoil`) 
			VALUES (NOW(), '".$tO."','".$hO."', '".$tI."', '".$hI."', '".$tS."', '".$hS."')"; 
   	
		//echo $query;
		mysqli_query($link, $query);
		mysqli_close($link);
	}
?>
