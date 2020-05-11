const { spawn } = require('child_process')
const express = require('express')
const app = express()

let runPy = new Promise(function(success, nosuccess) {
  const process = spawn('python3', ['./python/testNode.py'])
  process.stdout.on('data', function(data) {
    success(data)
  })
  process.stderr.on('data', (data) => {
    nosuccess(data)
  })
})

app.get('/', (req, res) => {
  runPy.then(fromRunpy => {
    console.log(fromRunpy.toString())
    res.sendStatus(200)
  }).catch(e => {
    res.status(500).send(e)
  })
})

app.listen(8082, () => console.log('Server Started'))
