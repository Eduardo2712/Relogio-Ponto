<?php
include("conexao.php");
$nome = $_POST['nome'];
$consulte = "SELECT * FROM digital WHERE idPessoa IS NULL";
// print_r($consulte);
// exit();
$verifica = $mysqli->query($consulte) or die("erro ao selecionar");
$verifica = $verifica->fetch_array();
// print_r($verifica);
// exit();
if (count($verifica) <= 0) {
  echo "<script language='javascript' type='text/javascript'>
        alert('Cadastre uma digital');window.location
        .href='login.html';</script>";
  die();
} else {
  $consulte = "SELECT MAX(idDigital) FROM digital";
  // print_r($consulte);
  // exit();
  $verifica = $mysqli->query($consulte) or die("erro ao selecionar");
  $verifica = $verifica->fetch_array();
  $maiorDigital = $verifica[0];
  $consulte = "INSERT INTO pessoa (nome) VALUES ('{$nome}')";
  $verifica = $mysqli->query($consulte) or die("erro ao selecionar");
  //$verifica = $verifica->fetch_array();
  $consulte = "SELECT MAX(id) FROM pessoa";
  // print_r($consulte);
  // exit();
  $verifica = $mysqli->query($consulte) or die("erro ao selecionar");
  $verifica = $verifica->fetch_array();
  $maiorPessoa = $verifica[0];
  $consulte = "UPDATE digital SET idPessoa = '{$maiorPessoa}' WHERE idDigital = '{$maiorDigital}'";
  // print_r($consulte);
  // exit();
  $verifica = $mysqli->query($consulte) or die("erro ao cadastrar");
  //$verifica = $verifica->fetch_array();
  //setcookie("login", $login);
  header("Location:LowPrice.php");
}
