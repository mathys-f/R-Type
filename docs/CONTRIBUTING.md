If you want to add a new page, here are the steps to follow.

Add a new md or mdx file in the /docs/docs folder.

On the top of said file, add this header:

```bash
---
title: Contributing as a new Guy    # Visible in the sidebar & as the title of the page
id: contributing-new-guy            # Id used to reference the page in the sidebar, must be lower case with hiphens only.
---
```

Finally, add your page to the sidebar.ts file.