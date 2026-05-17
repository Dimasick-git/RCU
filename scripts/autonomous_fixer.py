import os
import requests
import json
import subprocess
import sys

# Configuration
OPENROUTER_API_KEY = os.getenv("OPENROUTER_API_KEY")
GITHUB_TOKEN = os.getenv("GITHUB_TOKEN")
REPO = os.getenv("GITHUB_REPOSITORY")
RUN_ID = os.getenv("FAILED_RUN_ID")
REF = os.getenv("GITHUB_REF_NAME") # Current branch name
MODEL = "openrouter/free"

def run_command(command, cwd=None):
    result = subprocess.run(command, shell=True, capture_output=True, text=True, cwd=cwd)
    return result.stdout.strip(), result.stderr.strip(), result.returncode

def get_failed_logs():
    if not RUN_ID: return "No run ID."
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

def ask_ai(prompt, system_prompt):
    url = "https://openrouter.ai/api/v1/chat/completions"
    headers = {"Authorization": f"Bearer {OPENROUTER_API_KEY}", "Content-Type": "application/json"}
    payload = {
        "model": MODEL,
        "messages": [{"role": "system", "content": system_prompt}, {"role": "user", "content": prompt}],
        "response_format": { "type": "json_object" }
    }
    response = requests.post(url, headers=headers, data=json.dumps(payload))
    return response.json()["choices"][0]["message"]["content"]

def main():
    if not OPENROUTER_API_KEY or not GITHUB_TOKEN:
        print("Missing API keys")
        return

    print(f"Autonomous fix for branch: {REF}")
    logs = get_failed_logs()
    
    system_prompt = """
    You are an autonomous AI engineer. Fix the error in the repository.
    CRITICAL: DO NOT break "Ryazha-Авто" or "VRR".
    Return JSON: {"explanation": "...", "patch": "full content of the fixed file", "target_file": "path/to/file"}
    """
    
    user_prompt = f"Analyze these logs and provide the full content of the file that needs fixing:\n{logs}"
    
    print("Asking AI for fix...")
    ai_response = json.loads(ask_ai(user_prompt, system_prompt))
    
    explanation = ai_response.get("explanation")
    content = ai_response.get("patch")
    target_file = ai_response.get("target_file")
    
    if content and target_file:
        print(f"Applying fix to {target_file}: {explanation}")
        
        # Write file directly
        os.makedirs(os.path.dirname(target_file), exist_ok=True)
        with open(target_file, "w") as f:
            f.write(content)
            
        # Commit and push directly to the current branch
        run_command(f'git config --global user.name "AI Autonomous Fixer"')
        run_command(f'git config --global user.email "ai-fixer@manus.im"')
        run_command(f"git add {target_file}")
        run_command(f'git commit -m "AI Autonomous Fix: {explanation}"')
        
        # Use PAT for pushing if available
        remote_url = f"https://x-access-token:{GITHUB_TOKEN}@github.com/{REPO}.git"
        run_command(f"git remote set-url origin {remote_url}")
        stdout, stderr, code = run_command(f"git push origin {REF}")
        
        if code == 0:
            print("Successfully pushed fix to branch!")
        else:
            print(f"Failed to push: {stderr}")

if __name__ == "__main__":
    main()
