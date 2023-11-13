export default (cents) => {
  return `￥${(cents / 100).toFixed(2)}`
}
