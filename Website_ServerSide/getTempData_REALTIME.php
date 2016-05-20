<?php

include("connect.php"); 	
	
$link=Connection();
$result = mysqli_query($link, "select time, tempOutside, tempInside, tempSoil from ( select time, tempOutside, tempInside, tempSoil from microfarm0climate order by time desc limit 50 ) sub order by time asc");    
 
    $table = array();
    $table['cols'] = array(
    array('id' => "", 'label' => 'time', 'pattern' => "", 'type' => 'string'),
    array('id' => "", 'label' => 'Temperature Outside', 'pattern' => "", 'type' => 'number'),
	array('id' => "", 'label' => 'Temperature Inside', 'pattern' => "", 'type' => 'number'),
	array('id' => "", 'label' => 'Temperature Soil', 'pattern' => "", 'type' => 'number')
    );
    $rows = array();
    while ($nt = $result->fetch_assoc())
    {
    $temp = array();
    $temp[] = array('v' => $nt['time'], 'f' =>NULL);
    $temp[] = array('v' => $nt['tempOutside'], 'f' =>NULL);
	$temp[] = array('v' => $nt['tempInside'], 'f' =>NULL);
	$temp[] = array('v' => $nt['tempSoil'], 'f' =>NULL);
    $rows[] = array('c' => $temp);
    }
    $table['rows'] = $rows;
    $jsonTable = json_encode($table);
    echo $jsonTable;
?>