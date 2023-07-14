<?php
  include 'database.php';
  
  //---------------------------------------- Condition to check that POST value is not empty.
  if (!empty($_POST)) {
    // keep track post values
    $id = $_POST['id'];
    
    $myObj = (object)array();
    
    //........................................ 
    $pdo = Database::connect();
    $sql = 'SELECT * FROM esp32_table_test WHERE id="' . $id . '"';
    foreach ($pdo->query($sql) as $row) {
      $myObj->id = $row['id'];
      $myObj->LED_01 = $row['LED_01'];
      $myObj->LED_02 = $row['LED_02'];
      
      $myJSON = json_encode($myObj);
      
      echo $myJSON;
    }
    Database::disconnect();
    //........................................ 
  }
  //---------------------------------------- 
?>