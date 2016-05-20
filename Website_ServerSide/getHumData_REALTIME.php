<?php

include("connect.php"); 	
	
$link=Connection();
$result = mysqli_query($link, "select time, humOutside, humInside, humSoil from ( select time, humOutside, humInside, humSoil from microfarm0climate order by time desc limit 50 ) sub order by time asc");    
 
    $table = array();
    $table['cols'] = array(
    array('id' => "", 'label' => 'time', 'pattern' => "", 'type' => 'string'),
    array('id' => "", 'label' => 'Humidity Outside', 'pattern' => "", 'type' => 'number'),
	array('id' => "", 'label' => 'Humidity Inside', 'pattern' => "", 'type' => 'number'),
	array('id' => "", 'label' => 'Humidity Soil', 'pattern' => "", 'type' => 'number')
    );
    $rows = array();
    while ($nt = $result->fetch_assoc())
    {
    $temp = array();
    $temp[] = array('v' => $nt['time'], 'f' =>NULL);
    $temp[] = array('v' => $nt['humOutside'], 'f' =>NULL);
	$temp[] = array('v' => $nt['humInside'], 'f' =>NULL);
	$temp[] = array('v' => $nt['humSoil'], 'f' =>NULL);
    $rows[] = array('c' => $temp);
    }
    $table['rows'] = $rows;
    $jsonTable = json_encode($table);
    echo $jsonTable;
?>