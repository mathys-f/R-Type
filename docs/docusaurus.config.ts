import {themes as prismThemes} from 'prism-react-renderer';
import type {Config} from '@docusaurus/types';
import type * as Preset from '@docusaurus/preset-classic';

// This runs in Node.js - Don't use client-side code here (browser APIs, JSX...)

const config: Config = {
  title: 'R-Type Documentation',
  favicon: 'img/favicon.jpg',

  // Future flags, see https://docusaurus.io/docs/api/docusaurus-config#future
  future: {
    v4: true, // Improve compatibility with the upcoming Docusaurus v4
  },

  // Set the production url of your site here
  url: 'https://your-docusaurus-site.example.com',
  // Set the /<baseUrl>/ pathname under which your site is served
  // For GitHub pages deployment, it is often '/<projectName>/'
  baseUrl: '/R-Type/',

  // GitHub pages deployment config.
  // If you aren't using GitHub pages, you don't need these.
  organizationName: 'mathys-f', // Usually your GitHub org/user name.
  projectName: 'R-Type', // Usually your repo name.

  onBrokenLinks: 'throw',

  // Even if you don't use internationalization, you can use this field to set
  // useful metadata like html lang. For example, if your site is Chinese, you
  // may want to replace "en" with "zh-Hans".
  i18n: {
    defaultLocale: 'en',
    locales: ['en'],
  },

  presets: [
    [
      'classic',
      {
        docs: {
          sidebarPath: './sidebars.ts',
          routeBasePath: '/',
        },
        blog: false,
        theme: {
          customCss: 'custom.css',
        },
      } satisfies Preset.Options,
    ],
  ],

  themeConfig: {
    colorMode: {
      respectPrefersColorScheme: true,
    },
    navbar: {
      title: 'R-Type Docs',
      logo: {
        alt: 'R-Type Logo',
        src: 'img/favicon.jpg',
      },
      items: [
        {
          type: 'docSidebar',
          sidebarId: 'setupSidebar',
          position: 'left',
          label: 'Setup',
        },
        {
          type: 'docSidebar',
          sidebarId: 'architectureSidebar',
          position: 'left',
          label: 'Architecture',
        },
        {
          type: 'docSidebar',
          sidebarId: 'gameplaySidebar',
          position: 'left',
          label: 'Game Design',
        },
        {
          type: 'docSidebar',
          sidebarId: 'networkingSidebar',
          position: 'left',
          label: 'Networking',
        },
        {
          type: 'docSidebar',
          sidebarId: 'contributingSidebar',
          position: 'left',
          label: 'Contributing',
        },
        {
          href: 'https://github.com/mathys-f/R-Type',
          label: 'GitHub',
          position: 'right',
        },
      ],
    },
    footer: {
      style: 'dark',
      links: [
        {
          title: 'Docs',
          items: [
            {
              label: 'Setup',
              to: '/setup/',
            },
            {
              label: 'Architecture',
              to: '/architecture/',
            },
            {
              label: 'Game Design',
              to: '/gameplay/',
            },
            {
              label: 'Networking',
              to: '/networking/',
            },
            {
              label: 'Contributing',
              to: '/contributing/',
            },
          ],
        },
        {
          title: 'More',
          items: [
            {
              label: 'GitHub',
              href: 'https://github.com/mathys-f/R-Type',
            },
          ],
        },
      ],
      copyright: `Copyright ©2025 R-Type, Inc. Built with Docusaurus.`,
    },
    prism: {
      theme: prismThemes.github,
      darkTheme: prismThemes.dracula,
    },
  } satisfies Preset.ThemeConfig,
};

export default config;
