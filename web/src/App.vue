<script setup>
import { computed, ref, watch } from 'vue'
import { useWindowSize } from '@vueuse/core'
import { useAxios } from '@vueuse/integrations/useAxios'
import { TransitionRoot, TransitionChild, Dialog, DialogPanel } from '@headlessui/vue'
import axios from 'axios'

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
  for (const i in goodTypes.value) {
    const typeInfo = goodTypes.value[i]

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

const goodTypes = ref([])

const getGoodTypeInfo = (typeId) => {
  for (const typeInfo of goodTypes.value) {
    if (typeInfo.id === Number(typeId)) {
      return typeInfo
    }
  }
  return null
}

const cashTypes = ref([])

const getCashTypeInfo = (value) => {
  for (const typeInfo of cashTypes.value) {
    if (typeInfo.value === Number(value)) {
      return typeInfo
    }
  }
  return null
}

const goodCounts = ref({})

const {
  data,
  execute: fetchData,
  isLoading: dataLoading,
  error: dataError
} = useAxios('/api/state', {}, {
  immediate: true,
  initialData: {
    inventory: [],
    cashBox: [],
    userBalance: 0
  },
  onSuccess: (data) => {
    goodTypes.value = data.goods || []
    cashTypes.value = data.cashes || []
    if (data.machine?.inventory) {
      for (const item of data.machine.inventory) {
        goodCounts.value[item.type] = item.quantity
      }
    }
  }
})

watch(goodTypes, () => drawPriceTags(), {
  immediate: true
})

const showOpsModal = ref(false)

const isProcessing = ref(false)

const insertCash = (value) => {
  if (isProcessing.value) {
    return
  }
  showOpsModal.value = false
  isProcessing.value = true

  axios.post('/api/cash', {
    value
  }, {
    headers: {
      'Content-Type': 'application/x-www-form-urlencoded'
    }
  }).then(() => {
    fetchData()
  }).catch((e) => {
    console.error(e)
    messageModalTitle.value = '投币失败'
    messageModalMessage.value = '请求遇到未知问题，请检查程序是否在运行。'
    showMessageModal.value = true
  }).finally(() => {
    isProcessing.value = false
  })
}

const isDropping = ref(false)

const showPickupModal = ref(false)

const itemsToPickup = ref({})

const totalItemsToPickup = computed(() => {
  let total = 0
  for (const i in itemsToPickup.value) {
    total += itemsToPickup.value[i]
  }
  return total
})

const animateDrop = (id) => {
  const eleAll = document.querySelectorAll(`.vm .vm-rack-good[data-item-id="${id}"]`)
  const ele = eleAll[eleAll.length - 1]

  const containerHeight = ele.parentElement.offsetHeight
  // original distance to the bottom of the container
  const eleBottomBase = containerHeight - ele.offsetTop - ele.offsetHeight
  // distance to the bottom of the container after dropping
  const eleBottomTarget = containerHeight * 0.2
  // theoretical drop distance in meters (suppose the height of the machine is 2m)
  const dropDistanceTotal = (eleBottomBase / containerHeight - 0.2) * 2
  const scale = dropDistanceTotal / (eleBottomBase - eleBottomTarget)

  let timeStart = null
  const animationFrame = (time) => {
    if (!timeStart) {
      timeStart = time
    }
    const timeElapsed = (time - timeStart) / 1000

    // distance dropped in meters
    const dropDistance = 9.81 * Math.pow(timeElapsed, 2) / 2
    if (dropDistance >= dropDistanceTotal) {
      ele.style.display = 'none'
      isDropping.value = false
      fetchData()
      return
    }

    // distance to the bottom of the container now
    const eleBottom = eleBottomBase - dropDistance / scale
    ele.style.bottom = Math.floor(eleBottom) + 'px'
    window.requestAnimationFrame(animationFrame)
  }

  isDropping.value = true
  ele.classList.add('vm-falling')
  setTimeout(() => {
    timeStart = null
    window.requestAnimationFrame(animationFrame)
  }, 500)
}

const purchaseGood = (id) => {
  if (isProcessing.value) {
    return
  }
  showOpsModal.value = false
  isProcessing.value = true

  axios.post('/api/purchase', {
    id
  }, {
    headers: {
      'Content-Type': 'application/x-www-form-urlencoded'
    }
  }).then(() => {
    itemsToPickup.value[id] = (itemsToPickup.value[id] || 0) + 1
    animateDrop(id)
  }).catch((e) => {
    messageModalTitle.value = '购买失败'
    showMessageModal.value = true
    if (e.response?.status === 402) {
      messageModalMessage.value = e.response.data
    } else {
      console.error(e)
      messageModalMessage.value = '请求遇到未知问题，请检查程序是否在运行。'
    }
  }).finally(() => {
    isProcessing.value = false
  })
}

const openPickupModal = () => {
  if (isDropping.value) {
    messageModalTitle.value = '取货'
    messageModalMessage.value = '正在出货，请等待出货完成后再取货。'
    showMessageModal.value = true
  } else if (!totalItemsToPickup.value) {
    messageModalTitle.value = '取货'
    messageModalMessage.value = '当前没有待取商品。'
    showMessageModal.value = true
  } else {
    showPickupModal.value = true
  }
}

const closePickupModal = () => {
  showPickupModal.value = false
  itemsToPickup.value = {}
}

const changes = ref([])

const showChangesModal = ref(false)

const makeChanges = () => {
  if (isProcessing.value) {
    return
  }
  showOpsModal.value = false
  isProcessing.value = true

  axios.post('/api/exit').then(({ data }) => {
    changes.value = data
    showChangesModal.value = true
    fetchData()
  }).catch((e) => {
    messageModalTitle.value = '找零失败'
    showMessageModal.value = true
    if (e.response?.status === 402) {
      messageModalMessage.value = e.response.data
    } else {
      console.error(e)
      messageModalMessage.value = '请求遇到未知问题，请检查程序是否在运行。'
    }
  }).finally(() => {
    isProcessing.value = false
  })
}

const closeChangesModal = () => {
  showChangesModal.value = false
  changes.value = []
}

const refill = () => {
  if (isProcessing.value) {
    return
  }

  if (isDropping.value) {
    messageModalTitle.value = '补货'
    messageModalMessage.value = '正在出货，请等待出货完成后再补货。'
    showMessageModal.value = true
    return
  }

  confirmModalCallback = () => {
    isProcessing.value = true
    axios.post('/api/refill').then(() => {
      fetchData()
      messageModalTitle.value = '补货完成'
      messageModalMessage.value = ''
      showMessageModal.value = true
    }).catch((e) => {
      console.error(e)
      messageModalTitle.value = '补货失败'
      messageModalMessage.value = '请求遇到未知问题，请检查程序是否在运行。'
      showMessageModal.value = true
    }).finally(() => {
      isProcessing.value = false
    })
  }

  showOpsModal.value = false
  confirmModalMessage.value = '你正在进行补货操作，此操作会将每种商品余量补至 10 件，并在零钱箱中补充 10 张一元现金和 10 张五角现金用于找零。确定要继续吗？'
  showConfirmModal.value = true
}

const messageModalTitle = ref('')

const messageModalMessage = ref('')

const showMessageModal = ref(false)

const confirmModalMessage = ref('')

const showConfirmModal = ref(false)

let confirmModalCallback = null

const confirmModalConfirm = () => {
  if (confirmModalCallback) {
    confirmModalCallback()
    confirmModalCallback = null
  }
  showConfirmModal.value = false
}

const isLock = computed(() => {
  return isLoading.value || isDropping.value
})

const isLoading = computed(() => {
  return dataLoading.value || isProcessing.value
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
        v-for="(typeInfo, i) in goodTypes"
        :key="i"
      >
        <tippy>
          <div
            class="absolute rounded hover:bg-white/10"
            style="left: 7%; width: 66.5%; height: 9.2%; z-index: 1"
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
            class="vm-rack-good absolute object-contain transform pointer-events-none"
            :alt="typeInfo.name"
            :src="getImageUrl(typeInfo.imageId + '-small')"
            style="width: 6.384%; height: 8%; z-index: 2"
            :style="{ bottom: (84.3 - 9.575 * i) + '%', left: 7 + 66.5 * ((j - 1) * 0.1 + 0.002) + '%' }"
            :data-item-id="typeInfo.id"
          />
          <img
            class="vm-rack-ring absolute h-auto pointer-events-none"
            alt="货架"
            :src="getImageUrl('ring')"
            style="width: 6.384%; z-index: 3"
            :style="{ bottom: (84.3 - 9.575 * i) + '%', left: 7 + 66.5 * ((j - 1) * 0.1 + 0.002) + '%' }"
            :data-item-id="typeInfo.id"
          />
        </template>
      </template>
      <div
        class="absolute rounded-sm hover:ring-4 ring-amber-500 cursor-pointer"
        style="bottom: 16.7%; left: 18.7%; width: 42.5%; height: 17.5%; z-index: 6"
        v-tippy="'取货口'"
        @click="openPickupModal()"
      />
      <div
        class="absolute rounded-lg hover:ring-4 ring-amber-500 cursor-pointer"
        style="bottom: 40.1%; left: 81.2%; width: 14.4%; height: 48.6%; z-index: 6"
        v-tippy="'操作区'"
        @click="showOpsModal = true"
      />
      <div
        class="absolute rounded-sm hover:ring-4 ring-amber-500 cursor-pointer"
        style="bottom: 32.8%; left: 93.2%; width: 4.2%; height: 6.2%; z-index: 6"
        v-tippy="'补货'"
        @click="refill"
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
      <div class="flex flex-col gap-2 text-sm font-medium">
        <p
          v-if="data.machine"
          class="text-green-700"
        >
          已投金额：{{ money(data.machine.userBalance || 0) }}
        </p>
        <p
          v-if="isLoading"
          class="text-indigo-600"
        >
          正在加载数据...
        </p>
        <p
          v-else-if="dataError"
          class="text-red-600"
        >
          加载数据失败！请检查程序是否在运行并刷新重试。
        </p>
        <p
          v-if="isDropping"
          class="text-sky-600"
        >
          正在出货，请耐心等待...
        </p>
        <p
          v-else-if="totalItemsToPickup > 0"
          class="text-sky-600"
        >
          你有 {{ totalItemsToPickup }} 件待取商品，请点击取货口取货。
        </p>
      </div>
      <hr class="border-t border-gray-400 my-3">
      <div class="flex flex-col gap-1 text-xs text-gray-500">
        <p class="text-base text-gray-600">
          使用说明
        </p>
        <ol class="list-decimal list-inside">
          <li>点击售卖机右侧操作区进行投币和选择商品。</li>
          <li>确认购买后，在下方取物口拿取购买的商品。</li>
          <li>结束购买时，在右侧操作区操作找零。</li>
        </ol>
      </div>
    </div>
    <transition-root
      appear
      :show="showOpsModal"
      as="template"
    >
      <Dialog
        as="div"
        @close="showOpsModal = false"
        class="relative z-10"
      >
        <transition-child
          as="template"
          enter="duration-300 ease-out"
          enter-from="opacity-0"
          enter-to="opacity-100"
          leave="duration-200 ease-in"
          leave-from="opacity-100"
          leave-to="opacity-0"
        >
          <div
            class="fixed inset-0 bg-black/30"
            aria-hidden="true"
          />
        </transition-child>

        <div class="fixed inset-0 overflow-y-auto">
          <div class="flex min-h-full items-center justify-center p-4">
            <transition-child
              as="template"
              enter="duration-300 ease-out"
              enter-from="opacity-0 scale-95"
              enter-to="opacity-100 scale-100"
              leave="duration-200 ease-in"
              leave-from="opacity-100 scale-100"
              leave-to="opacity-0 scale-95"
            >
              <dialog-panel class="w-full max-w-xl transform overflow-hidden rounded-2xl bg-white align-middle shadow-xl transition-all">
                <div class="flex items-stretch border-b border-gray-300">
                  <div class="flex-1 p-6 border-r border-gray-300">
                    <h3 class="mb-4 text-xl text-center font-medium text-gray-900">
                      投币
                    </h3>
                    <div class="flex flex-col items-stretch gap-2">
                      <button
                        v-for="(typeInfo, i) of cashTypes"
                        :key="i"
                        type="button"
                        class="flex-1 inline-flex items-center gap-1 rounded-md bg-blue-100 p-2 text-sm focus:outline-none select-none"
                        :class="{ 'cursor-not-allowed text-gray-500': isLock, 'text-blue-900 hover:bg-blue-200': !isLock }"
                        :disabled="isLock"
                        @click="insertCash(typeInfo.value)"
                      >
                        <img
                          class="w-8 h-5 object-contain flex-shrink-0"
                          :src="getImageUrl(typeInfo.imageId)"
                          :alt="typeInfo.name"
                        />
                        <span class="flex-1">投{{ typeInfo.name }}</span>
                      </button>
                    </div>
                  </div>
                  <div class="flex-1 p-6 border-r border-gray-300">
                    <h3 class="mb-4 text-xl text-center font-medium text-gray-900">
                      选择商品
                    </h3>
                    <div class="flex flex-col items-stretch gap-2">
                      <button
                        v-for="(typeInfo, i) in goodTypes"
                        :key="i"
                        class="flex-1 inline-flex items-center gap-1 rounded-md bg-blue-100 p-2 text-sm focus:outline-none select-none"
                        :class="{ 'cursor-not-allowed text-gray-500': isLock, 'text-blue-900 hover:bg-blue-200': !isLock }"
                        :disabled="isLock"
                        @click="purchaseGood(typeInfo.id)"
                      >
                        <img
                          class="w-4 h-5 object-contain flex-shrink-0"
                          :src="getImageUrl(typeInfo.imageId)"
                          :alt="typeInfo.name"
                        />
                        <span class="flex-1">买{{ typeInfo.name }}</span>
                      </button>
                    </div>
                  </div>
                  <div class="flex-1 p-6">
                    <h3 class="mb-4 text-xl text-center font-medium text-gray-900">
                      其它操作
                    </h3>
                    <div class="flex flex-col items-stretch gap-2">
                      <button
                        type="button"
                        class="flex-1 rounded-md bg-blue-100 p-2 text-sm focus:outline-none select-none"
                        :class="{ 'cursor-not-allowed text-gray-500': isLock, 'text-blue-900 hover:bg-blue-200': !isLock }"
                        :disabled="isLock"
                        @click="makeChanges"
                      >
                        找零并退出
                      </button>
                      <button
                        type="button"
                        class="flex-1 rounded-md bg-blue-100 p-2 text-sm focus:outline-none select-none"
                        :class="{ 'cursor-not-allowed text-gray-500': isLock, 'text-blue-900 hover:bg-blue-200': !isLock }"
                        :disabled="isLock"
                        @click="refill"
                      >
                        补货
                      </button>
                    </div>
                  </div>
                </div>
                <div class="px-6 py-3">
                  <ul class="text-gray-500 text-xs space-y-1">
                    <li>
                      此系统为模拟售卖机，投币和购物操作均不会实际扣费，请放心使用。
                    </li>
                  </ul>
                </div>
              </dialog-panel>
            </transition-child>
          </div>
        </div>
      </Dialog>
    </transition-root>
    <transition-root
      appear
      :show="showPickupModal"
      as="template"
    >
      <Dialog
        as="div"
        @close="closePickupModal"
        class="relative z-10"
      >
        <transition-child
          as="template"
          enter="duration-300 ease-out"
          enter-from="opacity-0"
          enter-to="opacity-100"
          leave="duration-200 ease-in"
          leave-from="opacity-100"
          leave-to="opacity-0"
        >
          <div
            class="fixed inset-0 bg-black/30"
            aria-hidden="true"
          />
        </transition-child>

        <div class="fixed inset-0 overflow-y-auto">
          <div class="flex min-h-full items-center justify-center p-4">
            <transition-child
              as="template"
              enter="duration-300 ease-out"
              enter-from="opacity-0 scale-95"
              enter-to="opacity-100 scale-100"
              leave="duration-200 ease-in"
              leave-from="opacity-100 scale-100"
              leave-to="opacity-0 scale-95"
            >
              <dialog-panel class="w-full max-w-md transform overflow-hidden rounded-2xl bg-white align-middle shadow-xl transition-all">
                <div class="flex flex-col items-stretch gap-4 p-6">
                  <h3 class="text-xl font-medium text-gray-900">
                    取货
                  </h3>
                  <p class="text-sm text-gray-600">
                    你获得了：
                  </p>
                  <div class="flex flex-col items-stretch space-y-4">
                    <template
                      v-for="(count, typeId) in itemsToPickup"
                      :key="typeId"
                    >
                      <div
                        v-if="count"
                        class="flex items-center gap-2"
                      >
                        <img
                          class="w-10 h-10 object-contain flex-shrink-0"
                          :src="getImageUrl(getGoodTypeInfo(typeId).imageId)"
                          :alt="getGoodTypeInfo(typeId).name"
                        />
                        <p class="text-lg text-gray-700">
                          {{ getGoodTypeInfo(typeId).name }}
                          <span class="text-sm text-gray-500">
                            x{{ count }}
                          </span>
                        </p>
                      </div>
                    </template>
                  </div>
                  <div class="flex justify-end">
                    <button
                      type="button"
                      class="rounded-md bg-blue-100 text-blue-900 hover:bg-blue-200 px-4 py-2 text-sm focus:outline-none select-none"
                      @click="closePickupModal"
                    >
                      确定
                    </button>
                  </div>
                </div>
              </dialog-panel>
            </transition-child>
          </div>
        </div>
      </Dialog>
    </transition-root>
    <transition-root
      appear
      :show="showChangesModal"
      as="template"
    >
      <Dialog
        as="div"
        @close="closeChangesModal"
        class="relative z-10"
      >
        <transition-child
          as="template"
          enter="duration-300 ease-out"
          enter-from="opacity-0"
          enter-to="opacity-100"
          leave="duration-200 ease-in"
          leave-from="opacity-100"
          leave-to="opacity-0"
        >
          <div
            class="fixed inset-0 bg-black/30"
            aria-hidden="true"
          />
        </transition-child>

        <div class="fixed inset-0 overflow-y-auto">
          <div class="flex min-h-full items-center justify-center p-4">
            <transition-child
              as="template"
              enter="duration-300 ease-out"
              enter-from="opacity-0 scale-95"
              enter-to="opacity-100 scale-100"
              leave="duration-200 ease-in"
              leave-from="opacity-100 scale-100"
              leave-to="opacity-0 scale-95"
            >
              <dialog-panel class="w-full max-w-md transform overflow-hidden rounded-2xl bg-white align-middle shadow-xl transition-all">
                <div class="flex flex-col items-stretch gap-4 p-6">
                  <h3 class="text-xl font-medium text-gray-900">
                    谢谢惠顾
                  </h3>
                  <template v-if="changes.length">
                    <p class="text-sm text-gray-600">
                      你获得了以下找零：
                    </p>
                    <div class="flex flex-col items-stretch space-y-2">
                      <template
                        v-for="(changeInfo, i) in changes"
                        :key="i"
                      >
                        <div
                          v-if="changeInfo.quantity"
                          class="flex items-center gap-4"
                        >
                          <img
                            class="w-14 h-10 object-contain flex-shrink-0"
                            :src="getImageUrl(getCashTypeInfo(changeInfo.value).imageId)"
                            :alt="getCashTypeInfo(changeInfo.value).name"
                          />
                          <p class="text-lg text-gray-700">
                            {{ getCashTypeInfo(changeInfo.value).name }}
                            <span class="text-sm text-gray-500">
                            x{{ changeInfo.quantity }}
                          </span>
                          </p>
                        </div>
                      </template>
                    </div>
                  </template>
                  <p class="text-sm text-gray-600">
                    感谢使用自动售卖机，欢迎下次光临！
                  </p>
                  <p
                    v-if="totalItemsToPickup"
                    class="text-sm text-gray-600"
                  >
                    别忘了在取货口取走你的商品！
                  </p>
                  <div class="flex justify-end">
                    <button
                      type="button"
                      class="rounded-md bg-blue-100 text-blue-900 hover:bg-blue-200 px-4 py-2 text-sm focus:outline-none select-none"
                      @click="closeChangesModal"
                    >
                      确定
                    </button>
                  </div>
                </div>
              </dialog-panel>
            </transition-child>
          </div>
        </div>
      </Dialog>
    </transition-root>
    <transition-root
      appear
      :show="showMessageModal"
      as="template"
    >
      <Dialog
        as="div"
        @close="showMessageModal = false"
        class="relative z-10"
      >
        <transition-child
          as="template"
          enter="duration-300 ease-out"
          enter-from="opacity-0"
          enter-to="opacity-100"
          leave="duration-200 ease-in"
          leave-from="opacity-100"
          leave-to="opacity-0"
        >
          <div
            class="fixed inset-0 bg-black/30"
            aria-hidden="true"
          />
        </transition-child>

        <div class="fixed inset-0 overflow-y-auto">
          <div class="flex min-h-full items-center justify-center p-4">
            <transition-child
              as="template"
              enter="duration-300 ease-out"
              enter-from="opacity-0 scale-95"
              enter-to="opacity-100 scale-100"
              leave="duration-200 ease-in"
              leave-from="opacity-100 scale-100"
              leave-to="opacity-0 scale-95"
            >
              <dialog-panel class="w-full max-w-md transform overflow-hidden rounded-2xl bg-white align-middle shadow-xl transition-all">
                <div class="flex flex-col items-stretch gap-4 p-6">
                  <h3
                    v-if="messageModalTitle"
                    class="text-xl font-medium text-gray-900"
                    v-text="messageModalTitle"
                  />
                  <p
                    v-if="messageModalMessage"
                    class="text-sm text-gray-600"
                    v-text="messageModalMessage"
                  />
                  <div class="flex justify-end gap-4">
                    <button
                      type="button"
                      class="rounded-md bg-blue-100 text-blue-900 hover:bg-blue-200 px-4 py-2 text-sm focus:outline-none select-none"
                      @click="showMessageModal = false"
                    >
                      确定
                    </button>
                  </div>
                </div>
              </dialog-panel>
            </transition-child>
          </div>
        </div>
      </Dialog>
    </transition-root>
    <transition-root
      appear
      :show="showConfirmModal"
      as="template"
    >
      <Dialog
        as="div"
        @close="showConfirmModal = false"
        class="relative z-10"
      >
        <transition-child
          as="template"
          enter="duration-300 ease-out"
          enter-from="opacity-0"
          enter-to="opacity-100"
          leave="duration-200 ease-in"
          leave-from="opacity-100"
          leave-to="opacity-0"
        >
          <div
            class="fixed inset-0 bg-black/30"
            aria-hidden="true"
          />
        </transition-child>

        <div class="fixed inset-0 overflow-y-auto">
          <div class="flex min-h-full items-center justify-center p-4">
            <transition-child
              as="template"
              enter="duration-300 ease-out"
              enter-from="opacity-0 scale-95"
              enter-to="opacity-100 scale-100"
              leave="duration-200 ease-in"
              leave-from="opacity-100 scale-100"
              leave-to="opacity-0 scale-95"
            >
              <dialog-panel class="w-full max-w-md transform overflow-hidden rounded-2xl bg-white align-middle shadow-xl transition-all">
                <div class="flex flex-col items-stretch gap-4 p-6">
                  <h3 class="text-xl font-medium text-gray-900">
                    注意
                  </h3>
                  <p
                    class="text-sm text-gray-600"
                    v-text="confirmModalMessage"
                  />
                  <div class="flex justify-end gap-4">
                    <button
                      type="button"
                      class="rounded-md bg-gray-200 text-gray-900 hover:bg-gray-300 px-4 py-2 text-sm focus:outline-none select-none"
                      @click="showConfirmModal = false"
                    >
                      取消
                    </button>
                    <button
                      type="button"
                      class="rounded-md bg-blue-100 text-blue-900 hover:bg-blue-200 px-4 py-2 text-sm focus:outline-none select-none"
                      @click="confirmModalConfirm"
                    >
                      确定
                    </button>
                  </div>
                </div>
              </dialog-panel>
            </transition-child>
          </div>
        </div>
      </Dialog>
    </transition-root>
  </div>
</template>
