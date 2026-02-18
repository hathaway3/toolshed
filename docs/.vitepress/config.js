export default {
  title: 'ToolShed',
  description: 'A cornucopia of tools for the Tandy Color Computer and Dragon micro.',
  themeConfig: {
    nav: [
      { text: 'Home', link: '/' },
      { text: 'Guide', link: '/guide/tutorial' },
      { text: 'Tools', link: '/tools/os9' }
    ],
    sidebar: [
      {
        text: 'Introduction',
        items: [
          { text: 'Overview', link: '/introduction' },
          { text: 'Building ToolShed', link: '/guide/building' },
          { text: 'Disk Image Tutorial', link: '/guide/tutorial' }
        ]
      },
      {
        text: 'Tools',
        items: [
          { text: 'os9', link: '/tools/os9' },
          { text: 'decb', link: '/tools/decb' },
          { text: 'cecb', link: '/tools/cecb' },
          { text: 'mamou', link: '/tools/mamou' },
          { text: 'ar2', link: '/tools/ar2' },
          { text: 'cocofuse', link: '/tools/cocofuse' }
        ]
      },
      {
        text: 'Firmware',
        items: [
          { text: 'HDB-DOS', link: '/firmware/hdbdos' },
          { text: 'DriveWire DOS', link: '/firmware/dwdos' },
          { text: 'SuperDOS', link: '/firmware/superdos' }
        ]
      }
    ],
    socialLinks: [
      { icon: 'github', link: 'https://github.com/nitros9project/toolshed' }
    ]
  }
}
