import React from 'react';
import * as projects from '../../services/projectsService'
import { Paginator, Modal } from '../common/index'
import ProjectCreationForm  from './ProjectCreationForm'
import Icon, {  UploadOutlined, UserOutlined } from '@ant-design/icons';
import { Spinner, Toast, ToastHeader, ToastBody, Badge } from 'reactstrap'

export default class MyProjects extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            user: props.user,
            projects: undefined,
            pagesCount: 0,
            itemsPerPage: 0,
            currentPage: 1,
            highlightedProjectId: undefined
        }
    }

    loadProjects = (page=1) => {
        console.log("loading projects");
        const { user } = this.state;
        projects.loadProjectList(user, page).then(
            res => {
                // console.log("Successful projects loading", res);
                const responseData = res.data;
                const projects = JSON.parse(responseData.results);
                this.setState({
                    projects: projects,
                    pagesCount: Math.ceil(responseData.count / projects.length),
                    currentPage: page,
                });
            }).catch(error => {
                console.log("Error projects loading");
                console.error(error.response);
                console.error(error.response.data);
            });
    };

    componentDidMount() {
        this.loadProjects();
    }

    handlePageChange = value => {
        console.log("changing to ", value)
        this.loadProjects(value);
    };

    handleProjectCreated = id => {
        console.log("created project ", id)
        this.setState({
            highlightedProjectId: id,
        });
        this.loadProjects();
    };

    renderSingleProject(project) {
        const { highlightedProjectId } = this.state;
        return (
            <Toast
                className={`m-3 rounded project-info${project.id === highlightedProjectId ? " highlighted" : ""}`}
                key={project.id}
            >
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
        const { user, projects, currentPage, pagesCount } = this.state;
        if (projects === undefined) {
            return <Spinner/>
        }
        return (
            <React.Fragment>
                <ProjectCreationForm
                    user={user}
                    onProjectCreated={this.handleProjectCreated}
                />
                <div className="row d-flex flex-wrap">
                    {this.renderProjects()}
                </div>
                <Paginator
                    className={"row d-flex justify-content-center py-auto"}
                    page={currentPage}
                    count={pagesCount}
                    onPageChange={this.handlePageChange}
                />

            </React.Fragment>
        );
    }
};