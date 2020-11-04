<?php
include("conexao.php");
$login = $_POST['login'];
$entrar = $_POST['entrar'];
$senha = md5($_POST['senha']);
if (isset($entrar)) {
  $consulte = "SELECT * FROM usuario WHERE login LIKE
'{$login}' AND senha LIKE '{$senha}'";
  // print_r($consulte);
  // exit();
  $verifica = $mysqli->query($consulte) or die("erro ao selecionar");
  $verifica = $verifica->fetch_array();
  // print_r($verifica);
  // exit();
  if (count($verifica) <= 0) {
    echo "<script language='javascript' type='text/javascript'>
        alert('Login e/ou senha incorretos');window.location
        .href='login.html';</script>";
    die();
  } else {
    setcookie("login", $login);
    header("Location:LowPrice.php");
  }
}
