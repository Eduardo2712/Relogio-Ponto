<?php
include("conexao.php");
$consulta = "SELECT * FROM pessoa p INNER JOIN horarioponto h on h.idPessoa = p.id;";
$con = $mysqli->query($consulta) or die($mysqli->error);
?>
<html>
<heade>
    <meeta charset="utf8">
        <title>LOWPRICE</title>
        <link rel="stylesheet" type="text/css" href="style.css">
        <link href="https://fonts.googleapis.com/css2?family=Merriweather+Sans:wght@400;500;600;700&display=swap" rel="stylesheet">

</heade>

<body>
    <header>
        <div class="contHeader">
            <div>
                <h1>
                    <div class="price">LOW</div>
                    <div class="low">PRICE</div>
                </h1>
            </div>
            <div>
                <img alt="rs" src="sazed.png" />
            </div>
        </div>
    </header>
    <div class="cont">
        <h2>Adicionar um novo funcionário ao <strong>sistema</strong></h2>
        <div class="logsenha">
      <label>Novo Funcionário:</label><input type="text" name="login" id="login" />
    </div>
        </div>
        
        </body>

</html>