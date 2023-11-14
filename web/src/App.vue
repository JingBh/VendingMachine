<script setup>
import { computed, ref } from 'vue'
import { useWindowSize } from '@vueuse/core'
import { useAxios } from '@vueuse/integrations/useAxios'

import { GoodTypes } from './utils/goods.js'
import { getImageUrl } from './utils/images.js'
import money from './utils/money.js'

const windowSize = useWindowSize()

const machineDimensions = computed(() => {
  const maxWidth = Math.max(windowSize.width.value - 192, 0)
  const maxHeight = Math.max(windowSize.height.value - 64, 0)
  const aspectRatio = 736 / 1088 // based on the dimensions of the image

  const machineWidth = Math.min(maxWidth, maxHeight * aspectRatio)
  const machineHeight = Math.min(maxHeight, maxWidth / aspectRatio)

  return {
    width: machineWidth + 'px',
    height: machineHeight + 'px'
  }
})

const canvasPriceTag = ref(null)

const drawPriceTags = () => {
  const ele = canvasPriceTag.value
  if (!ele) {
    return
  }

  const ctx = ele.getContext('2d')
  ctx.clearRect(0, 0, ele.width, ele.height)

  let itemIndex = 0
  for (const i in GoodTypes) {
    const typeInfo = GoodTypes[i]

    const yT = ele.height * (0.157 + 0.09575 * i) + 2
    const yB = yT + 12

    for (let i = 0; i < 10; i++) {
      const xL = ele.width * (0.07 + 0.0665 * i)
      const xR = ele.width * (0.07 + 0.0665 * (i + 1))

      const num = (++itemIndex).toString().padStart(2, '0')
      ctx.fillStyle = '#d4d4d4' // gray-300
      ctx.fillRect(xL, yT, (xR - xL) / 3, yB - yT)
      ctx.fillStyle = 'black'
      ctx.font = 'bold 10px sans-serif'
      ctx.fillText(num, xL + 1, yB - 2)

      const price = money(typeInfo.price)
      ctx.fillStyle = '#f5f5f5'
      ctx.font = '10px sans-serif'
      ctx.fillText(price, xL + (xR - xL) / 3, yB - 2)
    }
  }
}

const goodCounts = ref({})

const {
  data,
  execute: fetchData,
  isLoading,
  error: loadingError
} = useAxios('/api/state', {}, {
  immediate: true,
  initialData: {
    inventory: [],
    cashBox: [],
    userBalance: 0
  },
  onSuccess: (data) => {
    if (data.inventory) {
      for (const item of data.inventory) {
        goodCounts.value[item.type] = item.quantity
      }
    }
  },
  onFinish: () => {
    drawPriceTags()
  }
})
</script>

<template>
  <div
    class="h-screen mx-auto p-8 flex gap-6 justify-center items-stretch"
    :style="{ width: machineDimensions.containerWidth }"
  >
    <div
      class="vm relative flex-shrink-0"
      :style="machineDimensions"
    >
      <img
        class="absolute top-0 left-0 w-full h-full pointer-events-none"
        alt="售卖机背景"
        :src="getImageUrl('vending-machine')"
        style="z-index: 5"
      />
      <img
        class="absolute top-0 left-0 w-full h-full pointer-events-none"
        alt="货架区背景"
        :src="getImageUrl('vending-machine-rack')"
        style="z-index: 0"
      />
      <template
        v-for="(typeInfo, i) in GoodTypes"
        :key="i"
      >
        <tippy>
          <div
            class="absolute rounded-lg hover:ring-2 ring-amber-500"
            style="left: 7%; width: 66.5%; height: 9.2%; z-index: 6"
            :style="{ bottom: (83 - 9.575 * i) + '%' }"
          />
          <template #content>
            <div class="flex flex-col gap-1">
              <p
                class="text-sm font-medium"
                v-text="typeInfo.name"
              />
              <p class="text-xs">
                单价：{{ money(typeInfo.price) }}
              </p>
              <p class="text-xs">
                剩余 {{ goodCounts[i] }} 件
              </p>
            </div>
          </template>
        </tippy>
        <template v-for="j in goodCounts[i]">
          <img
            class="vm-rack-good absolute object-contain"
            :alt="typeInfo.name"
            :src="getImageUrl(typeInfo.id + '-small')"
            style="width: 6.384%; height: 8%; z-index: 2"
            :style="{ bottom: (84.3 - 9.575 * i) + '%', left: 7 + 66.5 * ((j - 1) * 0.1 + 0.002) + '%' }"
          />
          <img
            class="vm-rack-ring absolute h-auto"
            alt="货架"
            :src="getImageUrl('ring')"
            style="width: 6.384%; z-index: 3"
            :style="{ bottom: (84.3 - 9.575 * i) + '%', left: 7 + 66.5 * ((j - 1) * 0.1 + 0.002) + '%' }"
          />
        </template>
      </template>
      <div
        class="absolute rounded-sm hover:ring-4 ring-amber-500 cursor-pointer"
        style="bottom: 16.7%; left: 18.7%; width: 42.5%; height: 17.5%; z-index: 6"
        v-tippy="'取货口'"
      />
      <div
        class="absolute rounded-sm hover:ring-4 ring-amber-500 cursor-pointer"
        style="bottom: 40.1%; left: 81.2%; width: 14.4%; height: 48.6%; z-index: 6"
        v-tippy="'操作区'"
      />
      <div
        class="absolute rounded-sm hover:ring-4 ring-amber-500 cursor-pointer"
        style="bottom: 32.8%; left: 93.2%; width: 4.2%; height: 6.2%; z-index: 6"
        v-tippy="'补货'"
      />
      <canvas
        class="absolute top-0 left-0 w-full h-full pointer-events-none"
        width="736"
        height="1088"
        ref="canvasPriceTag"
        style="z-index: 1"
      />
    </div>
    <div class="flex flex-col items-stretch gap-2" style="min-width: 128px; max-width: 256px">
      <div class="flex flex-col gap-1">
        <h1 class="text-xl text-gray-900">
          自动售卖机模拟系统
        </h1>
        <span class="text-sm text-gray-600">
          22080206&Tab;&nbsp;&nbsp;&nbsp;敬博浩
        </span>
      </div>
      <hr class="border-t border-gray-400 my-3">
      <div class="flex flex-col gap-1 text-sm">
        <p>已投金额：{{ money(data.userBalance || 0) }}</p>
        <p
          v-if="isLoading"
          class="text-blue-700"
        >
          正在加载数据...
        </p>
        <p
          v-else-if="loadingError"
          class="text-red-700"
        >
          加载数据失败！请检查程序是否在运行并刷新重试。
        </p>
      </div>
      <hr class="border-t border-gray-400 my-3">
      <div class="flex flex-col gap-1 text-xs text-gray-500">
        <p class="text-base text-gray-600">使用说明</p>
        <ol class="list-decimal list-inside">
          <li>点击售卖机右侧操作区进行投币和选择商品。</li>
          <li>确认购买后，在下方取物口拿取购买的商品。</li>
          <li>结束购买时，在右侧操作区操作找零。</li>
        </ol>
      </div>
    </div>
  </div>
</template>
