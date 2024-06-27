import { defineConfig } from 'astro/config';
import starlight from '@astrojs/starlight';

export default defineConfig({
	integrations: [
		starlight({
			title: 'The SCC Handbook',
			social: {
				github: 'https://github.com/scc-lang/scc',
			},
			sidebar: [
			],
		}),
	],
});
