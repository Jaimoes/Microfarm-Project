<?php

include("connect.php"); 	
	
$link=Connection();
$result = mysqli_query($link, "SELECT time, humOutside, humInside, humSoil FROM `Microfarm0Climate` ORDER BY `time` DESC");    
 
    $table = array();
    $table['cols'] = array(
    array('id' => "", 'label' => 'time', 'pattern' => "", 'type' => 'string'),
    array('id' => "", 'label' => 'humOutside', 'pattern' => "", 'type' => 'number'),
	array('id' => "", 'label' => 'humInside', 'pattern' => "", 'type' => 'number'),
	array('id' => "", 'label' => 'humSoil', 'pattern' => "", 'type' => 'number')
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