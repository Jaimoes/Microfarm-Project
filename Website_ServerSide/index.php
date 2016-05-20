<?php

	include("connect.php"); 	
	
	$link=Connection();

	$result = mysqli_query($link, "SELECT time, plantAge FROM `Microfarm0plantage` ORDER BY `time` DESC LIMIT 1");
	$plantAge = mysqli_fetch_assoc($result);
	mysqli_free_result($result);
	$result = mysqli_query($link, "SELECT time FROM `Microfarm0irrigationt` ORDER BY `time` DESC LIMIT 1");
	$pumpLastOn = mysqli_fetch_assoc($result);
	mysqli_free_result($result);
	mysqli_close($link);
	
	$date1 = new DateTime($plantAge['time']);
	$t=time();
	$date2 = new DateTime(date("Y-m-d H:i:s",$t));

	$interval = $date1->diff($date2);

?>

<html>
  <head>
  <title>Microfarm data</title>
      <!--Load the AJAX API-->
    <script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
    <script type="text/javascript" src="//ajax.googleapis.com/ajax/libs/jquery/1.10.2/jquery.min.js"></script>
    <script type="text/javascript">
	
      google.charts.load('current', {'packages':['corechart']});
      google.charts.setOnLoadCallback(drawChart);

      function drawChart() {
	  
	  
	    var jsonTempData = $.ajax({
        url: "getTempData.php",
        dataType: "json",
        async: false
        }).responseText;
		
		var jsonHumData = $.ajax({
        url: "getHumData.php",
        dataType: "json",
        async: false
        }).responseText;
		
        var tData = new google.visualization.DataTable(jsonTempData);	
		var hData = new google.visualization.DataTable(jsonHumData);		

		var width = $(window).width()
		
		if( width < 500){
			var width = 500;
		}
		
        var options = {
          title: 'Temperatures in microfarm0',
          //curveType: 'function',
		  'width': width,
          'height':300,
		  series: {
            0: { color: '#e2431e' },
            1: { color: '#e7711b' },
            2: { color: '#f1ca3a' },
		  },
          legend: { position: 'top' }
        };
		var options2 = {
          title: 'Humidity in microfarm0',
          //curveType: 'function',
		  'width':width,
          'height':300,
		  series: {
            0: { color: '#6f9654' },
            1: { color: '#1c91c0' },
            2: { color: '#43459d' },
		  },
          legend: { position: 'top' }
        };
		
		// Instantiate and draw our chart, passing in some options.
        var tChart = new google.visualization.LineChart(document.getElementById('temp_chart'));
		tChart.draw(tData, options);
		var hChart = new google.visualization.LineChart(document.getElementById('hum_chart'));
        hChart.draw(hData, options2);
      }
    </script>
  </head>
  <body>
    <div id="temp_chart"></div>
	<div id="hum_chart"></div>
	
	<table border="1" style="width:75%" cellspacing="1" align="center">
		<tr>
			<td>The current plant age is:</td>
			<td> <?php echo ($interval->days + $plantAge['plantAge']) . " days"; ?> </td>		
		</tr>
		<tr>
			<td>The last time the pump was on:</td>
			<td> <?php echo $pumpLastOn['time'];  ?> </td>		
		</tr>
	</table>
	<p>
	<form align="center" action="http://greensworth.nl/realtime.html">
			<input type="submit" value="Go to realtime page!">
	</form>
  </body>
</html>