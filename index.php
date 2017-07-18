<!DOCTYPE HTML>
<html>
<head>
<style>
.error {color: #FF0000;}
</style>
</head>
<body>

<?php
$nameError = $taskError = "";
$name = $task = "";
$target = $_SERVER["PHP_SELF"];

function test_input($data) {
  $data = trim($data);
  $data = stripslashes($data);
  $data = htmlspecialchars($data);
  return $data;
}
?>

<h1>Messaging System</h1>
<form method="post">
  Name: <input type="text" name="name" value="<?php echo $name;?>"><br>
  <br>
  <input type="radio" name="task" value="post" checked> Add Post<br>
  <input type="radio" name="task" value="view"> View Post<br>
  <input type="radio" name="task" value="author"> Add/Remove Author<br>
  <br>
  <input type="submit" name="submit" value="Submit">
</form>

<?php
if(isset($_POST['submit'])) {
  $name = test_input($_POST["name"]);
  $task = test_input($_POST["task"]);
  if ($name==="") {
    $target = $_SERVER["PHP_SELF"];
  } else if ($task==="author") {
    shell_exec("./generate_html author.wpml");
    $target = "Location: author.php?name=".$name."&source=".$_SERVER["PHP_SELF"];
  } else if ($task==="post") {
    shell_exec("./generate_html post.wpml");
    $target = "Location: post.php?name=".$name."&source=".$_SERVER["PHP_SELF"];
  } else if ($task==="view") {
    shell_exec("./generate_html view.wpml");
    $target = "Location: view.php?name=".$name."&source=".$_SERVER["PHP_SELF"];
  }
}
?>

<?php
header($target);
?>

</body>
</html>
