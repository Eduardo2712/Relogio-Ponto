const express = require('express');
const app = express();
const mysql = require('mysql');
const bodyparser = require('body-parser');
const morgan = require('morgan');

app.use(morgan('dev'));

var mysqlConnection = mysql.createConnection({
    host: 'localhost',
    user: 'root',
    password: '',
    database: 'bdnodemcu',
    multipleStatements: true
});

app.use(express.json());
var helmet = require('helmet');
app.use(helmet());

app.get('/espget/:nDigital', (req, res) => {
    mysqlConnection.query('SELECT p.id, p.nome FROM pessoa p INNER JOIN digital d WHERE d.idPessoa = p.id and d.posicaoDigital = ?;', [req.params.nDigital], (err, rows, fields) => {
        if (!err)
            res.send(rows);
        else
            console.log(err);
    })
});

app.get('/esppost/:idPessoa', (req, res) => {
    //idPessoa: idPessoa;
    mysqlConnection.query('INSERT INTO horarioponto (idPessoa) VALUES ("' + req.params.idPessoa + '")', (err, rows, fields) => {
        //idPessoa: idPessoa;
        if (!err)
            res.send("OK");
        else
            console.log(err);
    })
});

app.get('/espcad/:posicaoDigital', (req, res) => {
    //idPessoa: idPessoa;
    mysqlConnection.query('INSERT INTO digital (posicaoDigital) VALUES ("' + req.params.posicaoDigital + '")', (err, rows, fields) => {
        //idPessoa: idPessoa;
        if (!err)
            res.send("OK");
        else
            console.log(err);
    })
});

/*app.get('/espdigital/:posicao', (req, res) => {
    if (!err)
        res.send(rows);
    else
        console.log(err);
});*/

app.listen(8080, () => {
    console.log("Servidor http://localhost:8080/");
});