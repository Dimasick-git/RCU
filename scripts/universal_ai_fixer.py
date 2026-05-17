import os
import requests
import json
import subprocess
import sys
import re

# Configuration
OPENROUTER_API_KEY = os.getenv("OPENROUTER_API_KEY")
GITHUB_TOKEN = os.getenv("GITHUB_TOKEN")
REPO = os.getenv("GITHUB_REPOSITORY")
RUN_ID = os.getenv("FAILED_RUN_ID")
ISSUE_NUMBER = os.getenv("ISSUE_NUMBER")
MODEL = "openrouter/free" # Default to free router

def run_command(command, cwd=None):
    result = subprocess.run(command, shell=True, capture_output=True, text=True, cwd=cwd)
    return result.stdout.strip(), result.stderr.strip(), result.returncode

def get_failed_logs():
    if not RUN_ID: return "No run ID provided."
    headers = {"Authorization": f"token {GITHUB_TOKEN}"}
    url = f"https://api.github.com/repos/{REPO}/actions/runs/{RUN_ID}/jobs"
    response = requests.get(url, headers=headers)
    jobs = response.json().get("jobs", [])
    
    logs = ""
    for job in jobs:
        if job["conclusion"] == "failure":
            job_id = job["id"]
            log_url = f"https://api.github.com/repos/{REPO}/actions/jobs/{job_id}/logs"
            log_response = requests.get(log_url, headers=headers)
            logs += f"\nJob: {job['name']}\nLogs:\n{log_response.text[-10000:]}\n"
    return logs

def get_issue_context():
    if not ISSUE_NUMBER: return ""
    headers = {"Authorization": f"token {GITHUB_TOKEN}"}
    url = f"https://api.github.com/repos/{REPO}/issues/{ISSUE_NUMBER}"
    response = requests.get(url, headers=headers)
    issue_data = response.json()
    return f"Issue Title: {issue_data.get('title')}\nIssue Body: {issue_data.get('body')}"

def get_repo_context():
    structure, _, _ = run_command("find . -maxdepth 3 -not -path '*/.*'")
    recent_changes, _, _ = run_command("git log -n 5 --pretty=format:'%h - %s'")
    return f"Structure:\n{structure}\n\nRecent Changes:\n{recent_changes}"

def ask_ai(prompt, system_prompt):
    url = "https://openrouter.ai/api/v1/chat/completions"
    headers = {
        "Authorization": f"Bearer {OPENROUTER_API_KEY}",
        "Content-Type": "application/json",
        "HTTP-Referer": "https://github.com/Dimasick-git/RCU",
        "X-Title": "Universal AI Fixer"
    }
    
    payload = {
        "model": MODEL,
        "messages": [
            {"role": "system", "content": system_prompt},
            {"role": "user", "content": prompt}
        ],
        "response_format": { "type": "json_object" }
    }
    
    response = requests.post(url, headers=headers, data=json.dumps(payload))
    return response.json()["choices"][0]["message"]["content"]

def main():
    if not OPENROUTER_API_KEY or not GITHUB_TOKEN:
        print("Missing API keys")
        return

    print("Gathering context...")
    logs = get_failed_logs()
    issue_ctx = get_issue_context()
    repo_ctx = get_repo_context()
    
    system_prompt = """
    You are an expert AI Developer. Your goal is to fix ANY error in the repository.
    This includes: build errors, runtime bugs, configuration issues, or feature requests from issues.
    
    Rules:
    1. DO NOT break "Ryazha-Авто" or "VRR".
    2. Provide a clear explanation and a fix.
    3. Return JSON: {"explanation": "...", "patch": "unified diff or full file content", "target_file": "path/to/file"}
    """
    
    user_prompt = f"""
    Context:
    {repo_ctx}
    
    Failed Logs (if any):
    {logs}
    
    Issue/PR Context (if any):
    {issue_ctx}
    
    Analyze the situation and provide a fix. If it's a build error, fix the workflow or the code. 
    If it's an issue, implement the requested fix.
    """
    
    print("Asking AI...")
    ai_response = json.loads(ask_ai(user_prompt, system_prompt))
    
    explanation = ai_response.get("explanation")
    patch = ai_response.get("patch")
    target_file = ai_response.get("target_file")
    
    print(f"AI Suggestion: {explanation}")
    
    if patch and target_file:
        branch_name = f"ai-fix-{RUN_ID if RUN_ID else 'manual'}-{os.urandom(4).hex()}"
        run_command(f"git checkout -b {branch_name}")
        
        with open("fix.patch", "w") as f:
            f.write(patch)
            
        # Try to apply as patch first
        stdout, stderr, code = run_command("patch -p1 < fix.patch")
        if code != 0:
            # If patch fails, assume it's full content
            with open(target_file, "w") as f:
                f.write(patch)
        
        run_command("git add .")
        run_command(f'git commit -m "AI Fix: {explanation}"')
        run_command(f"git push origin {branch_name}")
        
        pr_title = f"AI Fix for {RUN_ID if RUN_ID else 'Issue #' + ISSUE_NUMBER}"
        pr_body = f"### AI Generated Fix\n\n**Explanation:**\n{explanation}\n\n**Applied Changes:**\n- Modified: `{target_file}`"
        
        run_command(f'gh pr create --title "{pr_title}" --body "{pr_body}" --base main --head {branch_name}')
        print(f"PR created successfully!")

if __name__ == "__main__":
    main()
