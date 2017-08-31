<?php
	$temp = (isset($_GET["temp"]) ? $_GET["temp"] : null);
	$humd = (isset($_GET["humd"]) ? $_GET["humd"] : null);
	
	echo "<b>This is the result: </b> <br />";
	echo "Temperature: "
	echo $temp;
	echo "<br />";
	echo "Humidity: ";
	echo $humd;
?>
