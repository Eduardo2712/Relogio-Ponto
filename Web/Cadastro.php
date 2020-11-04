<?php
include("conexao.php");
$login = $_POST['login'];
$cadastrar = $_POST['cadastrar'];
$senha = md5($_POST['senha']);
if (isset($cadastrar)) {
  $consulte = "SELECT * FROM usuario WHERE login = '{$login}'";
  // print_r($consulte);
  // exit();
  $verifica = $mysqli->query($consulte) or die("erro ao selecionar");
  $verifica = $verifica->fetch_array();
  // print_r($verifica);
  // exit();
  if (count($verifica) > 0) {
    echo "<script language='javascript' type='text/javascript'>
        alert('Login já existe');window.location
        .href='login.html';</script>";
    die();
  } else {
    $consulte = "INSERT INTO usuario (login, senha) VALUES ('{$login}', '{$senha}')";
    // print_r($consulte);
    // exit();
    $verifica = $mysqli->query($consulte) or die("erro ao cadastrar");
    //$verifica = $verifica->fetch_array();
    setcookie("login", $login);
    header("Location:login.html");
  }
}
