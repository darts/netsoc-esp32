const express = require('express')
const app = express()
const port = 8080
const wordArr = ['lol', 'netsoc', 'ESP32', 'events', 'test', 'lots of words', 'ERROR!!', 'HARDWARE FAILURE!']
let postedArr = ['lol', 'netsoc']


app.listen(port, (e) => { console.log(`Listening on port ${port}...`)})

function randWord(){
    return wordArr[Math.floor(Math.random() * wordArr.length)]
}

app.get('/', (req, res) =>{
    res.send(randWord())
})

app.get('/showPosts', (req, res) =>{
    res.setHeader('Content-Type', 'application/json')
    res.send(JSON.stringify(postedArr, null, 3))
})

app.get('/sendTextGet', (req, res) =>{
    if(req.query.string && req.query.string.length > 1){
        postedArr.push(req.query.string.substring(0,30))
        if(postedArr.length > 10)
            postedArr.shift()
        res.sendStatus(200)
    }else{
        res.sendStatus(400)
    }
})
