const Docker = require('dockerode')

const docker = new Docker()

const createContainer = async (uuid) => {
  if (!uuid) {
    return
  }

  const containers = await docker.listContainers({
    all: true,
    filters: {
      label: [
        'vending-machine-demo_type=machine',
        `vending-machine-demo_uuid=${uuid}`
      ]
    }
  })
  if (containers.length) {
    return
  }

  console.log(`Creating container for ${uuid}...`)

  const container = await docker.createContainer({
    Image: 'vending-machine-demo:machine',
    AttachStdin: false,
    AttachStdout: false,
    AttachStderr: false,
    Tty: true,
    OpenStdin: false,
    HostConfig: {
      NetworkMode: 'vending-machine-demo'
    },
    Labels: {
      'vending-machine-demo_type': 'machine',
      'vending-machine-demo_uuid': uuid,
      'traefik.enable': 'true',
      [`traefik.http.routers.machine-${uuid}.rule`]: `Host(\`${uuid}.vm.bjut.tech\`)`,
      [`traefik.http.routers.machine-${uuid}.entrypoints`]: 'web, websecure',
      [`traefik.http.routers.machine-${uuid}.priority`]: '20',
      [`traefik.http.routers.machine-${uuid}.tls`]: 'true',
      [`traefik.http.routers.machine-${uuid}.tls.certresolver`]: 'myresolver',
      [`traefik.http.routers.machine-${uuid}.tls.domains[0].main`]: 'vm.bjut.tech',
      [`traefik.http.routers.machine-${uuid}.tls.domains[0].sans`]: '*.vm.bjut.tech'
    }
  })

  await container.start()

  setTimeout(removeContainerByUuid, 3600 * 1000, uuid)
}

const removeContainerById = async (containerId) => {
  const container = docker.getContainer(containerId)
  await container.remove({
    force: true,
    v: true
  })
}

const removeContainerByUuid = async (uuid) => {
  const containers = await docker.listContainers({
    all: true,
    filters: {
      label: [
        'vending-machine-demo_type=machine',
        `vending-machine-demo_uuid=${uuid}`
      ]
    }
  })

  if (containers.length) {
    console.log(`Removing container for ${uuid}...`)
    await removeContainerById(containers[0].Id)
  }
}

const cleanupContainers = async () => {
  const containers = await docker.listContainers({
    all: true,
    filters: {
      label: [
        'vending-machine-demo_type=machine'
      ]
    }
  })

  console.log(`Cleaning up ${containers.length} containers...`)

  for (const container of containers) {
    await removeContainerById(container.Id)
  }

  console.log('Clean up complete')
}

module.exports = {
  cleanupContainers,
  createContainer,
  removeContainerById,
  removeContainerByUuid
}
