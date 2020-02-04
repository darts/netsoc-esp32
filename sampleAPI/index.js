const express = require('express')
const app = express()
const port = 8080
const wordArr = ['lol', 'netsoc', 'ESP32', 'events', 'test', 'lots of words', 'ERROR!!', 'HARDWARE FAILURE!']

app.get('/', (req, res) =>{
    res.send(randWord())
})

app.listen(port, (e) => { console.log(`Listening on port ${port}...`)})

function randWord(){
    return wordArr[Math.floor(Math.random() * wordArr.length)]
}
