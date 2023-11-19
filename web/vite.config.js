import fs from 'fs'
import path from 'path'

import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'
import { viteSingleFile } from 'vite-plugin-singlefile'
import { ViteMinifyPlugin } from 'vite-plugin-minify'

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [
    vue(),
    viteSingleFile(),
    ViteMinifyPlugin(),
    {
      // vite plugin to transform the index.html into a C++ header file
      name: 'vite-plugin-cpp-header',
      writeBundle(options, bundle) {
        for (const [fileName, fileInfo] of Object.entries(bundle)) {
          if (fileInfo.type === 'asset' && fileInfo.fileName === 'index.html') {
            const source = Buffer.from(fileInfo.source, 'utf-8')

            const output = '#include <cstddef>' + '\n\n' +
              `static constexpr unsigned char index_html[] = { ${source.join(', ')} };` + '\n\n' +
              `static constexpr std::size_t index_html_len = ${source.length};` + '\n'

            const distDir = options.dir || path.join(__dirname, 'dist')
            const includeDir = path.join(distDir, 'include')
            if (!fs.existsSync(includeDir)) {
              fs.mkdirSync(includeDir)
            }

            fs.writeFileSync(path.join(includeDir, 'index.html.h'), output)
          }
        }
      }
    }
  ],
  server: {
    proxy: {
      '/api': {
        target: 'http://localhost:8206',
        changeOrigin: true
      }
    }
  }
})
