<?php

	function Connection(){
		$server="localhost";
		$user="root";
		$pass="p0kemon!";
		$db="microfarm1";
	   	
		$connection = mysqli_connect($server, $user, $pass);

		if (!$connection) {
	    	die('MySQL ERROR: ' . mysqli_error());
		}
		
		mysqli_select_db($connection, $db) or die( 'MySQL ERROR: '. mysqli_error() );

		return $connection;
	}
?>
