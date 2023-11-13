import { createApp } from 'vue'
import VueTippy from 'vue-tippy'

import App from './App.vue'

import './assets/styles/app.css'
import 'tippy.js/dist/tippy.css'

const app = createApp(App)

app.use(VueTippy, {
  defaultProps: {
    placement: 'right-end',
    arrow: false,
    followCursor: true
  }
})

app.mount('#app')
