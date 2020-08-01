import React from 'react';
import * as projects from '../../services/projectsService'
import Icon, {  UploadOutlined, UserOutlined } from '@ant-design/icons';
import { Spinner, Toast, ToastHeader, ToastBody, Badge } from 'reactstrap'

export default class MyProjects extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            user: props.user,
            projects: undefined
        }
    }

    loadProjects = () => {
        const { user } = this.state;
        projects.loadProjectList(user)
                .then(res => {
                    console.log("Successful projects loading", res);
                    const data = JSON.parse(res.data);
                    this.setState({
                        projects: data
                    });
                })
                .catch(error => {
                    console.log("Error projects loading");
                    console.error(error.response);
                    console.error(error.response.data);
                })
    };

    componentDidMount() {
        this.loadProjects();
    }

    renderSingleProject(project) {
        return (
            <Toast className="m-3 rounded project-info" key={project.id}>
                <ToastHeader tag={props => <React.Fragment>{props.children}</React.Fragment>}>
                    <strong className="project-name col-8 text-left">{project.name}</strong>
                    <div className="col-4 p-0 ml-auto d-flex justify-content-end">
                        <span className="project-budget ml-auto d-flex align-items-center">
                            <Icon component={props => "$"} style={{marginRight: "0.1rem"}}/>
                            <span className="count">{project.budget_usd}</span>
                        </span>
                        <span className="project-employee-count d-flex align-items-center ml-sm-2">
                            <UserOutlined/>
                            <span className="count">{project.members_count}</span>
                        </span>
                    </div>
                </ToastHeader>
                <ToastBody>
                    {project.description}
                </ToastBody>
            </Toast>
        );
    }

    renderProjects() {
        const { projects } = this.state;
        return (
            <React.Fragment>
                {projects.map(proj => this.renderSingleProject(proj))}
            </React.Fragment>
        );
    }

    render() {
        const { projects } = this.state;
        if (projects === undefined) {
            return <Spinner/>
        }
        const { user } = this.state;
        return (
            <div className="row d-flex flex-wrap">
                <h3>Projects for user {user.token}</h3>
                {this.renderProjects()}
            </div>
        );
    }
};