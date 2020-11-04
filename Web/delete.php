<?php
include("conexao.php");
$delete = $_GET['delete'];
$consultaData = "DELETE FROM horarioponto where idHorarioPonto = '{$delete}'";
$con = $mysqli->query($consultaData) or die($mysqli->error);
/*$consulta = "SELECT * FROM pessoa p INNER JOIN horarioponto h on h.idPessoa = p.id";
$con = $mysqli->query($consulta) or die($mysqli->error);*/
echo "<meta HTTP-EQUIV='refresh' CONTENT='5;URL=Lowprice.php'>";
//echo json_encode($con->fetch_all());
