<?php
include("conexao.php");
$consulta = "SELECT * FROM pessoa p INNER JOIN horarioponto h on h.idPessoa = p.id;";
$con = $mysqli->query($consulta) or die($mysqli->error);
?>
<html>
<link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.3.1/css/all.css">
<header>
    <meta charset="utf8">
    <title>LOWPRICE</title>
    <link rel="stylesheet" type="text/css" href="style.css">
    <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.3.1/css/all.css">
    <link href="https://fonts.googleapis.com/css2?family=Merriweather+Sans:wght@400;500;600;700&display=swap" rel="stylesheet">

</header>

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
                if (isset($login_cookie)) {
                    echo "<h3 class='Edu'>Bem-Vindo  $login_cookie!<br><h3>";
                } else {
                    echo "Bem-Vindo, convidado <br>";
                    echo "Essas informações <font color='red'>NÃO PODEM</font> ser acessadas por você";
                    echo "<br><a href='login.html'>Faça Login</a> Para ler o conteúdo";
                }
                ?>
            </div>
            <div>
                <img alt="rs" src="sazed.png" />
            </div>
        </div>
    </header>
    <div class="cont">
        <h2>Controle de <strong>PONTO</strong></h2>
        <div class="tabelas">
            <div class="tabela0">
                <ul class=titulos>
                    <li class="name sazu">
                        <h3>NOMES</h3>
                    </li>
                    <li class="itens sazu">
                        <h3>HORÁRIO</h3>
                    </li>
                    <li class="itens sazu">
                        <h3>DATA</h3>
                    </li>
                    <li class="itens sazu">
                        <h3>EXCLUIR</h3>
                    </li>
                </ul>
                <div id="tabelinhajs">
                    <?php while ($dado = $con->fetch_array()) { ?>
                        <ul class="nome0">

                            <li class="name"><?php echo $dado["nome"]; ?></li>
                            <li class="itens"><?php echo date("H:i:s", strtotime($dado["dataHora"])); ?></li>
                            <li class="itens"><?php echo date("d/m/Y", strtotime($dado["dataHora"])); ?></li>
                            <li class="itens"><button class="busca" id="Delete-<?php echo $dado['idHorarioPonto'] ?>" data-id="<?php echo $dado['idHorarioPonto'] ?>">
                                    <i class="fas fa-trash-alt"></i>
                                </button></li>
                        </ul>
                    <?php  } ?>
                </div>

            </div>

        </div>
        <div class="botoes">
            <div class="dentro">
                <input placeholder="Nome" id="Pesquisa" class="input-search" type="text" />
                <button class="busca" id="Search">
                    BUSCAR
                </button>
            </div>
            <div class="dentro">
                <input placeholder="Dia" id="PesquisaDia" class="input-search" type="date" />
                <button class="busca" id="SearchDia">
                    BUSCAR
                </button>
            </div>
            <a href="LowPrice.php" class="todos">TODOS</a>
        </div>
        <div class="botaoCadastrar">
            <button onclick="window.location.href = 'CadastroPessoa.html'" class="cadastrar" id="cadastrar">
                CADASTRAR PESSOA
            </button>
            <div class="botaoCadastrar">
                <a href="cadastro.html" class="cadastrologin">NOVO USUÁRIO</a>
            </div>
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
                            let date = new Date(response[i][4]);
                            let xd = date.toLocaleDateString();
                            let time = date.toLocaleTimeString();
                            html += `<ul class="nome0">

                    <li class="name">${response[i][1]}</li>
                    <li class="itens">${time}</li>
                    <li class="itens">${xd}</li>
                    <li class="itens"><button class="busca" id="Delete-${response[i][2]}" 
                    data-id="${response[i][2]}">
                                    <i class="fas fa-trash-alt"></i>
                                </button></li>
                    </ul>`;

                        }

                        $('#tabelinhajs').html(html);

                    }
                })
            })
        })
        $(document).ready(function() {
            $('#SearchDia').on('click', function() {
                $.ajax({
                    url: 'pesqDia.php',
                    method: 'GET',
                    data: {
                        filtro: $('#PesquisaDia').val()
                    },
                    success: function(response) {
                        response = JSON.parse(response);
                        let html = '';
                        $('#tabelinhajs').html('');
                        console.log(response);
                        for (let i = 0; i < response.length; i++) {
                            let date = new Date(response[i][4]);
                            let xd = date.toLocaleDateString();
                            let time = date.toLocaleTimeString();
                            let del = response[i][0];
                            html += `<ul class="nome0">

                    <li class="name">${response[i][1]}</li>
                    <li class="itens">${time}</li>
                    <li class="itens">${xd}</li>
                    <li class="itens"><button class="busca" id="Delete-${response[i][2]}" 
                    data-id="${response[i][2]}">
                                    <i class="fas fa-trash-alt"></i>
                                </button></li>
                    </ul>`;

                        }

                        $('#tabelinhajs').html(html);

                    }
                })
            })

            $('.fa-trash-alt').on('click', function(event) {
                let element = event.target.closest('button')
                let id = $(`#${element.id}`).data('id');
                $.ajax({
                    url: `delete.php?delete=${id}`,
                    method: 'GET',
                    success: function(response) {
                        response = JSON.parse(response);
                        let html = '';
                        $('#tabelinhajs').html('');
                        console.log(response);
                        for (let i = 0; i < response.length; i++) {
                            let date = new Date(response[i][4]);
                            let xd = date.toLocaleDateString();
                            let time = date.toLocaleTimeString();
                            let del = response[i][0];
                            html += `<ul class="nome0">

                                <li class="name">${response[i][1]}</li>
                                <li class="itens">${time}</li>
                                <li class="itens">${xd}</li>
                                <li class="itens"><button class="busca"  id="Delete-${response[i][2]}" data-id="${response[i][2]}">
                                                <i class="fas fa-trash-alt"></i>
                                            </button></li>
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