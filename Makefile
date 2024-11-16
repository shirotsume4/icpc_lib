name: Verify library & Generate documentation

on:
  push:
    branches:
      - main
  pull_request:

permissions:
  contents: write

jobs:
  verify:
    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v4
      with:
        fetch-depth: 0

    - name: Set up Python
      uses: actions/setup-python@v5
      with:
        python-version: '3.x'
        cache: 'pip'

    - name: Install dependencies
      run: |
        pip install -U pip
        pip install -r requirements.txt

    - name: Switch to main branch
      run: git switch main

    - name: Set up Node.js
      uses: actions/setup-node@v4
      with:
        node-version: 'latest'
        cache: 'npm'

    - name: Install Vivliostyle
      run: npm install -g @vivliostyle/cli clang-format

    - name: Install dependencies
      run: npm install

    - name: Build PDF
      run: make build

    - name: Check build output
      run: ls -la build

    - name: Configure Git
      run: |
        git config --global user.email "github-actions[bot]@users.noreply.github.com"
        git config --global user.name "GitHub Actions Bot"

    - name: Commit and push PDF
      env:
        GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
      run: |
        git remote set-url origin https://x-access-token:${{ secrets.GITHUB_TOKEN }}@github.com/${{ github.repository }}
        git add .
        git commit -m "[skip actions] build PDF"
        git push

    - name: Ensure gh-pages branch exists
      run: |
        if ! git show-ref --quiet refs/heads/gh-pages; then
          git checkout --orphan gh-pages
          git rm -rf .
          echo "Initial commit for gh-pages branch" > index.html
          git add index.html
          git commit -m "Initial commit for gh-pages branch"
        else
          git checkout gh-pages
          git pull origin gh-pages --rebase
        fi
        git push origin gh-pages

    - name: Copy to gh-pages
      run: |
        DIR=$(mktemp -d)
        cp build/notebook.pdf $DIR
        cp build/*.css $DIR
        cp build/*.html $DIR
        git clean -dfx
        git switch gh-pages
        cp $DIR/* .
        git add .
        git commit -m "build PDF"
        git push
        git switch main
