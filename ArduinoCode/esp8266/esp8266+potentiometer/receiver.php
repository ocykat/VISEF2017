<?php
	$analogval = (isset($_GET["anlinp"]) ? $_GET["anlinp"] : null);
	
	$fileContent = "Analog input: $analogval\n";
	
	$fileStatus = file_put_contents("record.txt", $fileContent.FILE_APPEND);

	if ($fileStatus !== false) {
		echo "RECEIVED!";
	}
	else {
		echo "NOT RECEIVED!";
	}
?>