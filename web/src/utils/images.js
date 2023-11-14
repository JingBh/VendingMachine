import imgVendingMachine from '../assets/images/vending-machine.png?inline'
import imgVendingMachineRack from '../assets/images/vending-machine-rack.png?inline'
import imgRing from '../assets/images/ring.png?inline'
import imgCocaCola from '../assets/images/coca-cola.png?inline'
import imgCocaColaSmall from '../assets/images/coca-cola-small.png?inline'
import imgPepsiCola from '../assets/images/pepsi-cola.png?inline'
import imgPepsiColaSmall from '../assets/images/pepsi-cola-small.png?inline'
import imgOrangeJuice from '../assets/images/orange-juice.png?inline'
import imgOrangeJuiceSmall from '../assets/images/orange-juice-small.png?inline'
import imgCoffee from '../assets/images/coffee.png?inline'
import imgCoffeeSmall from '../assets/images/coffee-small.png?inline'
import imgWater from '../assets/images/water.png?inline'
import imgWaterSmall from '../assets/images/water-small.png?inline'

const imageUrls = {
  'vending-machine': imgVendingMachine,
  'vending-machine-rack': imgVendingMachineRack,
  ring: imgRing,
  'coca-cola': imgCocaCola,
  'coca-cola-small': imgCocaColaSmall,
  'pepsi-cola': imgPepsiCola,
  'pepsi-cola-small': imgPepsiColaSmall,
  'orange-juice': imgOrangeJuice,
  'orange-juice-small': imgOrangeJuiceSmall,
  coffee: imgCoffee,
  'coffee-small': imgCoffeeSmall,
  water: imgWater,
  'water-small': imgWaterSmall
}

export const getImageUrl = (name) => {
  return imageUrls[name]
}

export const getImageElement = (name) => {
  return new Promise((resolve, reject) => {
    const img = new Image()
    img.onload = () => resolve(img)
    img.onerror = reject
    img.src = imageUrls[name]
  })
}
