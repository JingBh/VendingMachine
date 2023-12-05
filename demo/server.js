const cookieParser = require('cookie-parser')
const express = require('express')
const { v4: uuid4 } = require('uuid')

const containerManager = require('./docker')
containerManager.cleanupContainers()


const app = express()
app.use(cookieParser())

app.get('/', (req, res) => {
  let userId = req.cookies['uid']
  if (!userId) {
    userId = uuid4()
    res.cookie('uid', userId, {
      maxAge: 3600 * 1000,
      domain: '.vm.bjut.tech'
    })
  }

  containerManager.createContainer(userId).then(() => {
    setTimeout((uuid) => {
      res.redirect(`//${uuid}.vm.bjut.tech`)
    }, 1000, userId)
  })
})

const PORT = 3000
app.listen(PORT, () => {
  console.log(`Listening on port ${PORT}`)
})
