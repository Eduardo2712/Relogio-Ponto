<?php
include("conexao.php");
$filtro = $_GET['filtro'];
$consulta = "SELECT * FROM pessoa p INNER JOIN horarioponto h on h.idPessoa = p.id where dataHora like '%{$filtro}%'";
$con = $mysqli->query($consulta) or die($mysqli->error);
echo json_encode($con->fetch_all());
