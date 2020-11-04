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
            <?php
  $login_cookie = $_COOKIE['login'];
    if(isset($login_cookie)){
      echo"Welcome  $login_cookie  !<br>";
      
    }else{
      echo"Bem-Vindo, convidado <br>";
      echo"Essas informações <font color='red'>NÃO PODEM</font> ser acessadas por você";
      echo"<br><a href='login.html'>Faça Login</a> Para ler o conteúdo";
    }
            ?>
            </div>
            <div>
                <img alt="rs" src="sazed.png" />
            </div>
        </div>
    </header>
    <div class="cont">
        <h2>Check <strong>POINT</strong></h2>
        <div class="tabelas">
            <div class="tabela0">
                <ul class=titulos>
                    <li class="name sazu">
                        <h3>NAMES</h3>
                    </li>
                    <li class="itens sazu">
                        <h3>TIMES</h3>
                    </li>
                    <li class="itens sazu">
                        <h3>DATE</h3>
                    </li>
                </ul>
                <div id="tabelinhajs">
                    <?php while ($dado = $con->fetch_array()) { ?>
                        <ul class="nome0">

                            <li class="name"><?php echo $dado["nome"]; ?></li>
                            <li class="itens"><?php echo date("H:i:s", strtotime($dado["dataHora"])); ?></li>
                            <li class="itens"><?php echo date("d/m/Y", strtotime($dado["dataHora"])); ?></li>

                        </ul>
                    <?php  } ?>
                </div>

            </div>

        </div>
        <div class="botoes">
            <input placeholder="Name" id="Pesquisa" class="input-search" type="text" />
            <button class="busca" id="Search">
            SEARCH
            </button>
            <a href="LowPrice.php" class="todos">ALL</a>
        </div>
    </div>
    <script type="text/javascript" src="jquery.js"></script>
    <script type="text/javascript" src="http://code.jquery.com/jquery-1.7.2.min.js"></script>
    <script>
        $(document).ready(function() {
            $('#Search').on('click', function() {
                $.ajax({
                    url: 'pesq.php',
                    method: 'GET',
                    data: {
                        filtro: $('#Pesquisa').val()
                    },
                    success: function(response) {
                        response = JSON.parse(response);
                        let html = '';
                        $('#tabelinhajs').html('');
                        console.log(response);
                        for (let i = 0; i < response.length; i++) {
                            let date = new Date(response[i][5]);
                            let xd = date.toLocaleDateString();
                            let time = date.toLocaleTimeString();
                            html += `<ul class="nome0">

                    <li class="name">${response[i][1]}</li>
                    <li class="itens">${time}</li>
                    <li class="itens">${xd}</li>
                    </ul>`;

                        }

                        $('#tabelinhajs').html(html);

                    }
                })
            })
        })
    </script>
</body>

</html>