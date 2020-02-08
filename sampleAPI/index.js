const express = require('express')
const app = express()
const port = 8080
const wordArr = ['lol', 'netsoc', 'ESP32', 'events', 'test', 'lots of words', 'ERROR!!', 'HARDWARE FAILURE!']
let postedArr = {}


app.listen(port, (e) => { console.log(`Listening on port ${port}...`) })

function randWord() {
    return wordArr[Math.floor(Math.random() * wordArr.length)]
}

app.get('/', (req, res) => {
    res.send(randWord())
})

app.get('/showPosts', (req, res) => {
    res.setHeader('Content-Type', 'application/json')
    if (req.query.id)
        res.send(JSON.stringify(postedArr[req.query.id]))
    else
        res.send(postedArr)

})

app.get('/sendText', (req, res) => {
    console.log({ str: req.query.string, strLen: req.query.string.length, id: req.query.id, idLen: req.query.id.length })
    if (req.query.string && req.query.string.length > 1 && req.query.id && req.query.id.length > 1) {
        postedArr[req.query.id.substring(0, 30)] = req.query.string.substring(0, 30)
        res.sendStatus(200)
    } else {
        res.sendStatus(400)
    }
})
